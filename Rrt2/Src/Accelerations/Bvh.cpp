#include "Rrt2/Accelerations/Bvh.hpp"
#include "Rrt2/Geometries/GeometryBase.hpp"
#include "Rrt2/Scene.hpp"
#include "Rrt2/PrimRef.hpp"
#include "Rrt2/BoundingBox.hpp"
#include "Rrt2/Accelerations/PackedRay.hpp"
#include "Rrt2/MathBasics.hpp"
#include <array>
#include <gsl/span>
#include <numeric>
#include <bit>

namespace rrt
{
    // 希望这里可以被 inline
    BoundingBox GetBounds(gsl::span<const PrimRefStorage> prims)
    {
        Float4 lower = GetInfinity();
        Float4 upper = GetNegInfinity();
        for (const PrimRefStorage& prim : prims)
        {
            const auto loaded = prim.Load();
            lower = Min(lower, loaded.lower);
            upper = Max(upper, loaded.upper);
        }
        const auto mask = MakeInts(-1, -1, -1, 0);
        lower = And(AsFloats(mask), lower);
        upper = And(AsFloats(mask), upper);
        return BoundingBox{.corners = {lower, upper}};
    }

    Bvh::Bvh(const Scene* scene) : m_scene{scene} {}

    std::optional<SurfaceInteraction> Bvh::Hit(const Ray& ray, float tMin, float tMax)
    {
        const PackedRay packedRay{ray};
        return m_root.Hit(packedRay, ray, tMin, tMax);
    }

    void Bvh::Build()
    {
        AlignedVec<PrimRefStorage> primRefs;
        for (const std::unique_ptr<GeometryBase>& geometry :
             m_scene->GetGeometries())
        {
            geometry->FillPrimitiveArray(primRefs);
        }
        PrimInfo root;
        for (const PrimRefStorage& prim : primRefs)
        {
            root.Extend(prim.Load());
        }
        m_root = Build(root, gsl::make_span(primRefs));
    }

    constexpr std::uint32_t kMaxLeafWidth = 4;

    NodeRef VECTORCALL Bvh::Build(PrimInfo primInfo,
                                  gsl::span<PrimRefStorage> prims)
    {
        PrimInfo childInfos[4];
        BinSplit split = Split(primInfo, prims, childInfos[0], childInfos[1]);
        childInfos[0] = primInfo;
        gsl::span<PrimRefStorage> childPrims[4];
        childPrims[0] = prims.subspan(0, split.splitPos);
        childPrims[1] = prims.subspan(split.splitPos);
        int currentChildrenCount = 2;

        while (currentChildrenCount < 4)
        {
            int childToSplit = 0;
            float maxArea = kNegInf;
            bool allLeaf = true;
            for (int i = 0; i < currentChildrenCount; ++i)
            {
                if (childPrims[i].size() <= kMaxSimdWidth)
                {
                    continue;
                }
                allLeaf = false;
                const float childArea =
                    childInfos[currentChildrenCount].geom.GetHalfArea();
                if (maxArea < childArea)
                {
                    maxArea = childArea;
                    childToSplit = i;
                }
            }
            if (allLeaf)
            {
                break;
            }
            gsl::span<PrimRefStorage> primsToSplit = childPrims[childToSplit];
            BinSplit childSplit = Split(childInfos[childToSplit], primsToSplit,
                                        childInfos[childToSplit],
                                        childInfos[currentChildrenCount]);
            childPrims[childToSplit] =
                primsToSplit.subspan(0, childSplit.splitPos);
            childPrims[currentChildrenCount] =
                primsToSplit.subspan(childSplit.splitPos);
            currentChildrenCount += 1;
        }
        InteriorNodeStorage* interiorNode = new InteriorNodeStorage;
        for (int i = 0; i < 4; ++i)
        {
            if (childPrims[i].empty())
            {
                // do nothing
                // this would create a childrenBox with inf, -inf
                // which would never be hit off
            }
            else if (childPrims[i].size() <= kMaxLeafWidth)
            {
                const gsl::span<const PrimRefStorage> childPrim = childPrims[i];
                Leaf* const leaf = new Leaf;
                const std::uint32_t simdRequired = PackedTriangle::GetSimdCount(
                    static_cast<std::uint32_t>(childPrim.size()));
                leaf->primitives.reserve(simdRequired);
                std::uint32_t start = 0;
                std::uint32_t end = static_cast<std::uint32_t>(childPrim.size());
                for (std::uint32_t j = 0; j < simdRequired && start < end; ++j)
                {
                    // PackedTriangle triangle;
                    PackedTriangleStorage& triangle =
                        leaf->primitives.emplace_back();
                    triangle.Fill(childPrim.data(), start, end, m_scene);
                }
                interiorNode->children[i] = NodeRef{leaf};
            }
            else
            {
                interiorNode->children[i] = Build(childInfos[i], childPrims[i]);
            }
            BoundingBoxStorage bboxStorage;
            childInfos[i].geom.StoreTo(bboxStorage);
            interiorNode->childrenBoxes.Set(i, bboxStorage);
        }
        return NodeRef{interiorNode};
    }

    static constexpr std::uint32_t kBinCount = 4;
    static constexpr std::uint32_t kDimCount = 3;

    struct BinInfo
    {
        // x, y, z

        BinInfo() { Clear(); }

        void Clear()
        {
            for (std::uint32_t i = 0; i < kDimCount; ++i)
            {
                for (std::uint32_t j = 0; j < kBinCount; ++j)
                {
                    binGeomBBoxes[i][j] = BoundingBoxStorage{
                        .corners = {glm::vec4{kInf}, glm::vec4{kNegInf}}};
                    binGeomCounts[i][j] = 0;
                }
            }
        }

        MDArray<BoundingBoxStorage, kDimCount, kBinCount> binGeomBBoxes;
        MDArray<std::uint32_t, kDimCount, kBinCount> binGeomCounts;
    };

    struct BinMapping
    {
        BinMapping(PrimInfo primInfo)
        {
            const Float4 width = primInfo.centroid.GetSize();
            ofs = primInfo.centroid.corners[0];
            //   const Float4 eps = MakeFloats(1.0f - 1E-34f);
            // 确保最边上的 primitive 不会产生 binid = 4
            const Float4 bias = MakeFloats(0.99f);
            const Float4 binCountVec =
                MakeFloats(static_cast<float>(kBinCount));
            const Float4 equalZero = Equal(width, ZeroFloats());
            scale = Select(equalZero, ZeroFloats(),
                           Div(Mul(binCountVec, bias), width));
        }

        Int4 CalcBinId(PrimRef primRef) const
        {
            const Float4 dist = Sub(primRef.GetCenter(), ofs);
            const Float4 floatId = Mul(scale, dist);
            return _mm_cvttps_epi32(floatId);
        }

        Float4 ofs, scale;
    };

    BinSplit VECTORCALL Bvh::Split(PrimInfo primInfo,
                                   gsl::span<PrimRefStorage> prims,
                                   PrimInfo& leftInfo, PrimInfo& rightInfo)
    {

        BinMapping binMapping{primInfo};
        BinInfo binInfo;

        for (const PrimRefStorage& primRefStorage : prims)
        {
            const PrimRef primRef = primRefStorage.Load();
            const Int4 binId = binMapping.CalcBinId(primRef);

            const auto updateBinInfo = [&](int dim, int dimBinId) {
                BoundingBoxStorage& binGeomBBoxStorage =
                    binInfo.binGeomBBoxes[dim][dimBinId];
                BoundingBox bbox = binGeomBBoxStorage.Load();
                binInfo.binGeomCounts[dim][dimBinId] += 1;
                bbox.Extend(BoundingBox{primRef.lower, primRef.upper});
                // TODO: NT Store here?
                bbox.StoreTo(binGeomBBoxStorage);
            };
            const int x = First(binId);
            const int y = Second(binId);
            const int z = Third(binId);
            updateBinInfo(0, x);
            updateBinInfo(1, y);
            updateBinInfo(2, z);
        }

        float rightScores[kDimCount][kBinCount - 1] = {};
        int bestBin[kDimCount];
        int bestDim;
        float cost = kInf;

        for (std::uint32_t i = 0; i < kDimCount; ++i)
        {
            float dimCost = kInf;

            BoundingBox rightBBox = BoundingBox::CreateInvalidBBox();

            for (std::int32_t r = kBinCount - 2; r >= 0; --r)
            {
                rightBBox.Extend(binInfo.binGeomBBoxes[i][r + 1].Load());
                rightScores[i][r] =
                    rightBBox.GetHalfArea() * binInfo.binGeomCounts[i][r + 1];
            }
            BoundingBox lBBox{.corners = {GetInfinity(), GetNegInfinity()}};

            for (std::int32_t l = 0; l < kBinCount - 1; ++l)
            {
                // Cost j = A L,j N L,j +A R,j N R,j
                // N : count of triangles
                // A: volume
                lBBox.Extend(binInfo.binGeomBBoxes[i][l].Load());
                const float leftScore =
                    lBBox.GetHalfArea() * binInfo.binGeomCounts[i][l];

                const float currentCost = leftScore + rightScores[i][l];
                if (currentCost < dimCost)
                {
                    bestBin[i] = l;
                    dimCost = currentCost;
                }
            }

            if (dimCost < cost)
            {
                cost = dimCost;
                bestDim = i;
            }
        }

        float splitPos;
        const Float4 domain =
            Mul(primInfo.centroid.GetSize(), MakeFloats(1.0f / kBinCount));
        // what about store domain/corner first?
        switch (bestDim)
        {
        case 0:
            splitPos = First(primInfo.centroid.corners[0]) +
                       (bestBin[0] + 1) * First(domain);
            break;
        case 1:
            splitPos = Second(primInfo.centroid.corners[0]) +
                       (bestBin[0] + 1) * Second(domain);
            break;
        case 2:
            splitPos = Third(primInfo.centroid.corners[0]) +
                       (bestBin[0] + 1) * Third(domain);
            break;
        default:
            RRT_UNREACHABLE;
            break;
        }
        leftInfo.Reset();
        rightInfo.Reset();
        const auto pivot = std::partition(
            prims.begin(), prims.end(),
            [&](const PrimRefStorage& primInfoStorage) {
                float centerPos = std::midpoint(primInfoStorage.lower[bestDim],
                                                primInfoStorage.upper[bestDim]);
                bool isLeft = centerPos < splitPos;
                if (isLeft)
                {
                    leftInfo.Extend(primInfoStorage.Load());
                }
                else
                {
                    rightInfo.Extend(primInfoStorage.Load());
                }
                return isLeft;
            });

        return BinSplit{.cost = cost,
                        .dim = bestDim,
                        .dist = splitPos,
                        .splitPos =
                            static_cast<std::size_t>(pivot - prims.begin())};
    }

    NodeRef::NodeRef(const Leaf* leaf)
        : m_nodeRef{reinterpret_cast<std::uintptr_t>(leaf)}
    {
        assert((m_nodeRef & kMask) == 0);
    }

    NodeRef::NodeRef(const InteriorNodeStorage* node)
        : m_nodeRef{reinterpret_cast<std::uintptr_t>(node) | kMask}
    {
        // FIXME
        assert((m_nodeRef & kMask) == 1);
    }

    NodeRef::NodeRef() : m_nodeRef{} {}

    std::optional<SurfaceInteraction> NodeRef::Hit(const PackedRay& packedRay,
                                          const Ray& ray, float tMin,
                                          float tMax) const
    {
        switch (GetKind())
        {
        case Kind::kNode:
            return GetInteriorNode()->Hit(packedRay, ray, tMin, tMax);
        case Kind::kLeaf:
            return GetLeaf()->Hit(packedRay, ray, tMin, tMax);
        default:
            assert(false);
            break;
        }
    }

    gsl::span<const PackedTriangleStorage> NodeRef::GetLeafPrimitives() const
    {
        assert(GetKind() == kLeaf);
        return gsl::make_span(
            reinterpret_cast<const Leaf*>(GetPtr())->primitives);
    }

    std::optional<SurfaceInteraction>
    InteriorNodeStorage::Hit(const PackedRay& packedRay, const Ray& ray,
                             float tMin, float tMax) const
    {
        std::uint32_t hitMask = childrenBoxes.Load().Hit(packedRay, tMin, tMax);
        if (hitMask == 0)
            return std::nullopt;

        // fast path for only one boundingbox is hit
        if ((hitMask & (hitMask - 1)) == 0)
        {
            const int hitIndex = std::countr_zero(hitMask);
            return children[hitIndex].Hit(packedRay, ray, tMin, tMax);
        }

        // slow path for multi boxes
        std::optional<SurfaceInteraction> hitRecord;
        for (;;)
        {
            unsigned long index = std::countr_zero(hitMask);
            if (index == sizeof(hitMask) * 8)
            {
                break;
            }
            hitMask &= (hitMask - 1);
            const std::optional<SurfaceInteraction> childRecord =
                children[index].Hit(packedRay, ray, tMin, tMax);
            if (!childRecord)
                continue;
            if (!hitRecord || hitRecord.value().t > childRecord.value().t)
                hitRecord = childRecord;
        }
        return hitRecord;
    }

    void InteriorNodeStorage::Clear()
    {
        BoundingBoxStorage invalidBBox = BoundingBoxStorage{
            .corners = {glm::vec4{kInf}, glm::vec4{kNegInf}}};
        for (int i = 0; i < 4; ++i)
        {
            childrenBoxes.Set(i, invalidBBox);
        }
    }

    std::optional<SurfaceInteraction> Leaf::Hit(const PackedRay& packedRay,
                                       const Ray& ray, float tMin,
                                       float tMax) const
    {
        std::optional<SurfaceInteraction> hitRecord;
        for (const PackedTriangleStorage& triangleStorage : primitives)
        {
            const PackedTriangle packedTriangle = triangleStorage.Load();
            const std::optional<SurfaceInteraction> childRecord =
                packedTriangle.Hit(packedRay, ray, tMin, tMax);
            if (!childRecord)
                continue;
            if (!hitRecord || hitRecord.value().t > childRecord.value().t)
                hitRecord = childRecord;
        }
        return hitRecord;
    }

    void VECTORCALL PrimInfo::Extend(PrimRef primRef)
    {
        centroid.Extend(primRef.GetCenter());
        geom.Extend(BoundingBox{.corners = {primRef.lower, primRef.upper}});
    }

} // namespace rrt
