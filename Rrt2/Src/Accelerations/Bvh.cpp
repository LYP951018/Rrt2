﻿#include "Bvh.hpp"
#include "Geometries/GeometryBase.hpp"
#include "Scene.hpp"
#include "PrimRef.hpp"
#include "BoundingBox.hpp"
#include "PackedRay.hpp"
#include <gsl/span>
#include <numeric>

namespace rrt
{
    // 希望这里可以被 inline
    BoundingBox GetBounds(gsl::span<const PrimRefStorage> prims)
    {
        Float4 lower = GetNegInfinity();
        Float4 upper = GetInfinity();
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

    std::optional<HitRecord> Bvh::Hit(const Ray& ray, float tMin, float tMax)
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

        m_root = Build(gsl::make_span(primRefs), m_rootBox);
    }

    NodeRef Bvh::Build(gsl::span<PrimRefStorage> prims,
                       BoundingBoxStorage& boundingBoxStorage)
    {
        if (prims.size() <= kMaxSimdWidth)
        {
            Leaf* const leaf = new Leaf;
            const std::uint32_t simdRequired = PackedTriangle::GetSimdCount(
                static_cast<std::uint32_t>(prims.size()));
            leaf->primitives.reserve(simdRequired);
            std::uint32_t start = 0;
            std::uint32_t end = static_cast<std::uint32_t>(prims.size());
            for (std::uint32_t i = 0; i < simdRequired && start < end; ++i)
            {
                // PackedTriangle triangle;
                PackedTriangleStorage& triangle =
                    leaf->primitives.emplace_back();
                triangle.Fill(prims.data(), start, end, m_scene);
            }
            return NodeRef{leaf};
        }
        const BoundingBox boundingBox = GetBounds(prims);
        const Float4 center = boundingBox.GetCenter();
        const float centerX = First(center);
        const float centerY = Second(center);
        //暂时使用 SSE，分成 4 份
        // FIXME: 使用递归实现任意份的分割
        const auto xPivot = std::partition(
            prims.begin(), prims.end(), [&](const PrimRefStorage& rhs) {
                return std::midpoint(rhs.lower.x, rhs.upper.x) > centerX;
            });

        const auto y1Pivot = std::partition(
            prims.begin(), xPivot, [&](const PrimRefStorage& rhs) {
                return std::midpoint(rhs.lower.y, rhs.upper.y) > centerY;
            });

        const auto y2Pivot =
            std::partition(xPivot, prims.end(), [&](const PrimRefStorage& rhs) {
                return std::midpoint(rhs.lower.y, rhs.upper.y) > centerY;
            });

        const std::ptrdiff_t x0 = y1Pivot - prims.begin();
        const std::ptrdiff_t x1 = xPivot - y1Pivot;
        const std::ptrdiff_t x2 = y2Pivot - xPivot;

        InteriorNodeStorage* const node = new InteriorNodeStorage;
        BoundingBoxStorage childBBox;
        const auto build = [&](std::ptrdiff_t start, std::ptrdiff_t count,
                               std::uint32_t index) {
            node->children[index] =
                Build(prims.subspan(start, count), childBBox);
            node->childrenBoxes.Set(index, childBBox);
        };
        build(0, x0, 0);
        build(x0, x1, 1);
        build(xPivot - prims.begin(), x2, 2);
        build(y2Pivot - prims.begin(), prims.end() - y2Pivot, 3);
        return NodeRef{node};
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
        assert((reinterpret_cast<std::uintptr_t>(node) & kMask) == 1);
    }

    NodeRef::NodeRef() : m_nodeRef{} {}

    std::optional<HitRecord> NodeRef::Hit(const PackedRay& packedRay,
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

    std::optional<HitRecord>
    InteriorNodeStorage::Hit(const PackedRay& packedRay,
                                               const Ray& ray, float tMin,
                                               float tMax) const
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
        std::optional<HitRecord> hitRecord;
        for (;;)
        {
            unsigned long index = std::countr_zero(hitMask);
            if (index == sizeof(hitMask) * 8)
            {
                break;
            }
            hitMask &= (hitMask - 1);
            const std::optional<HitRecord> childRecord =
                children[index].Hit(packedRay, ray, tMin, tMax);
            if (!childRecord)
                continue;
            if (!hitRecord || hitRecord.value().t > childRecord.value().t)
                hitRecord = childRecord;
        }
        return hitRecord;
    }

    std::optional<HitRecord> Leaf::Hit(const PackedRay& packedRay,
                                       const Ray& ray, float tMin,
                                       float tMax) const
    {
        std::optional<HitRecord> hitRecord;
        for (const PackedTriangleStorage& triangleStorage : primitives)
        {
            const PackedTriangle packedTriangle = triangleStorage.Load();
            const std::optional<HitRecord> childRecord =
                packedTriangle.Hit(packedRay, ray, tMin, tMax);
            if (!childRecord)
                continue;
            if (!hitRecord || hitRecord.value().t > childRecord.value().t)
                hitRecord = childRecord;
        }
        return hitRecord;
    }

} // namespace rrt