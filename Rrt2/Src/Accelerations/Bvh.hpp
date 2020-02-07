#pragma once

#include "AccelerationBase.hpp"
#include "BoundingBox.hpp"
#include "PackedTriangle.hpp"
#include "PackedBoundingBox.hpp"
#include "AlignedVec.hpp"
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

        std::optional<HitRecord> Hit(const PackedRay& packedRay, const Ray& ray, float tMin,
                                     float tMax) const;

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

        std::optional<HitRecord> Hit(const PackedRay& packedRay, const Ray& ray, float tMin,
                                     float tMax) const;
    };

    struct alignas(16) Leaf
    {
        AlignedVec<PackedTriangleStorage> primitives;

        std::optional<HitRecord> Hit(const PackedRay& packedRay, const Ray& ray, float tMin,
                                     float tMax) const;
    };

    class PrimRefStorage;

    struct Bvh : AccelerationBase
    {
      public:
        inline static constexpr std::uint32_t kMaxDepth = 16;

        Bvh(const Scene* scene);
        std::optional<HitRecord> Hit(const Ray& ray, float tMin, float tMax) override;
        void Build() override;
        // std::optional<HitRecord>

      private:
        NodeRef Build(gsl::span<PrimRefStorage> prims, BoundingBoxStorage& boundingBoxStorage);

        const Scene* m_scene;
        NodeRef m_root;
        BoundingBoxStorage m_rootBox;
    };
}

