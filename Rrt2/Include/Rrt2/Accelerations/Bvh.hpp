#pragma once

#include "AccelerationBase.hpp"
#include "../BoundingBox.hpp"
#include "PackedTriangle.hpp"
#include "PackedBoundingBox.hpp"
#include "../AlignedVec.hpp"
#include "../PrimRef.hpp"
#include <gsl/span>

namespace rrt
{
    class GeometryBase;

    struct InteriorNodeStorage;
    struct Leaf;
    class PackedRay;
    class Ray;

    struct NodeRef
    {
      public:
        enum Kind
        {
            kNode = 0,
            kLeaf
        };

      private:
        std::uintptr_t m_nodeRef;

        inline static constexpr std::uintptr_t kMask = 1;
        inline static constexpr std::uintptr_t kUnmask = ~kMask;

      public:
        NodeRef(const Leaf* leaf);
        NodeRef(const InteriorNodeStorage* node);
        NodeRef();

        Kind GetKind() const
        {
            return (m_nodeRef & kMask) == 0 ? kLeaf : kNode;
            // return m_leafCount == UINT32_MAX ? kNode : kLeaf;
        }

        std::optional<SurfaceInteraction> Trace(const PackedRay& packedRay) const;

        const InteriorNodeStorage* GetInteriorNode() const
        {
            assert(GetKind() == kNode);
            return reinterpret_cast<const InteriorNodeStorage*>(GetPtr());
        }

        const Leaf* GetLeaf() const
        {
            assert(GetKind() == kLeaf);
            return reinterpret_cast<const Leaf*>(GetPtr());
        }

        gsl::span<const PackedTriangleStorage> GetLeafPrimitives() const;

        const std::byte* GetPtr() const
        {
            return reinterpret_cast<const std::byte*>(m_nodeRef & kUnmask);
        }
    };

    struct alignas(16) InteriorNodeStorage
    {
        PackedBoundingBoxStorage childrenBoxes;
        NodeRef children[4];

        InteriorNodeStorage() { Clear(); }
        std::optional<SurfaceInteraction> Trace(const PackedRay& packedRay) const;

        void Clear();
    };

    struct alignas(16) Leaf
    {
        // FIXME: static_vector or something
        AlignedVec<PackedTriangleStorage> primitives;

        std::optional<SurfaceInteraction> Trace(const PackedRay& packedRay) const;
    };

    class PrimRefStorage;

    struct BinSplit
    {
        float cost;
        int dim; // bin 在哪一维分的
        float dist;
        std::size_t splitPos;
        //BinMapping binMapping;
    };

    struct PrimInfo
    {
        BoundingBox centroid; // 多边形中心形成的 bounding box，用于生成 bin
        BoundingBox geom; // 多边形本身的 bounding box，计算 SAH

        PrimInfo()
        {
            Reset();
        }
        void VECTORCALL Extend(PrimRef primRef);
        void Reset()
        {
            centroid = BoundingBox::CreateInvalidBBox();
            geom = BoundingBox::CreateInvalidBBox();
        }
    };

    struct Bvh : AccelerationBase
    {
      public:
        inline static constexpr std::uint32_t kMaxDepth = 16;

        Bvh(const Scene* scene);
        std::optional<SurfaceInteraction> Trace(const Ray& ray) override;
        bool Hit(const Ray& ray) override;
        void Build() override;

      private:
        NodeRef VECTORCALL Build(PrimInfo primInfo,
                                 gsl::span<PrimRefStorage> prims);
        BinSplit VECTORCALL Split(PrimInfo primInfo,
                                  gsl::span<PrimRefStorage> prims,
                                  PrimInfo& leftInfo, PrimInfo& rightInfo);
        // setup normals
        // ...
        void Postprocess(SurfaceInteraction& interaction) const;
        const Scene* m_scene;
        NodeRef m_root;
        BoundingBoxStorage m_rootBox;
    };
}

