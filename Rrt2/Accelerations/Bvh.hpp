﻿#pragma once

#include "AccelerationBase.hpp"
#include "../BoundingBox.hpp"
#include "SimdTriangle.hpp"
#include "SimdBoundingBox.hpp"
#include "../AlignedVec.hpp"
#include <gsl/span>

class GeometryBase;

struct InteriorNode;
struct Leaf;

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
    NodeRef(const InteriorNode* node);
    NodeRef();

    Kind GetKind() const
    {
        return (m_nodeRef & kMask) == 0 ? kLeaf : kNode;
        // return m_leafCount == UINT32_MAX ? kNode : kLeaf;
    }

    const InteriorNode* GetInteriorNode() const
    {
        assert(GetKind() == kNode);
        return reinterpret_cast<const InteriorNode*>(GetPtr());
    }

    gsl::span<const SimdTriangle> GetLeafPrimitives() const;

    const std::byte* GetPtr() const
    {
        return reinterpret_cast<const std::byte*>(m_nodeRef & kUnmask);
    }
};

struct alignas(16) InteriorNode
{
    SimdBoundingBox childrenBoxes;
    NodeRef children[8];
};

struct alignas(16) Leaf
{
    AlignedVec<SimdTriangle> primitives;
};

class PrimRef;

struct Bvh : AccelerationBase
{
  public:
    inline static constexpr std::uint32_t kMaxDepth = 16;

    Bvh(const Scene* scene);

    void Build() override;

  private:
    NodeRef Build(gsl::span<PrimRef> prims, BoundingBox& boundingBox);

    const Scene* m_scene;
    NodeRef m_root;
    BoundingBox m_rootBox;
};