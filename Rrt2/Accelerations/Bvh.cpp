#include "Bvh.hpp"
#include "../Geometries/GeometryBase.hpp"
#include "../Scene.hpp"
#include "../PrimRef.hpp"
#include "../BoundingBox.hpp"

BoundingBox GetBounds(gsl::span<const PrimRef> prims)
{
    BoundingBox box;
    Float4& lower = box.corners[0];
    Float4& upper = box.corners[1];
    lower = GetNegInfinity();
    upper = GetInfinity();
    for (const PrimRef& prim : prims)
    {
        lower = Min(lower, prim.lower);
        upper = Max(upper, prim.upper);
    }
    return box;
}

Bvh::Bvh(const Scene* scene) : m_scene{scene} {}

void Bvh::Build()
{
    AlignedVec<PrimRef> primRefs;
    for (const std::unique_ptr<GeometryBase>& geometry : m_scene->GetGeometries())
    {
        geometry->FillPrimitiveArray(primRefs);
    }

    m_root = Build(gsl::make_span(primRefs), m_rootBox);
}

NodeRef Bvh::Build(gsl::span<PrimRef> prims, BoundingBox& boundingBox)
{
    if (prims.size() <= kMaxSimdWidth)
    {
        Leaf* const leaf = new Leaf;
        const std::uint32_t simdRequired =
            SimdTriangle::GetSimdCount(static_cast<std::uint32_t>(prims.size()));
        leaf->primitives.reserve(simdRequired);
        std::uint32_t start = 0;
        std::uint32_t end = static_cast<std::uint32_t>(prims.size());
        for (std::uint32_t i = 0; i < simdRequired && start < end; ++i)
        {
            SimdTriangle triangle;
            triangle.Fill(prims.data(), start, end, m_scene);
            leaf->primitives.push_back(triangle);
        }
        return NodeRef{leaf};
    }
    boundingBox = GetBounds(prims);
    const Float4 center =
        Div(Add(boundingBox.corners[0], boundingBox.corners[1]), MakeFloats(2.0f));
    const float centerX = First(center);
    const float centerY = Second(center);
    //暂时使用 SSE，分成 4 份
    const auto xPivot = std::partition(prims.begin(), prims.end(), [&](const PrimRef& rhs) {
        return First(rhs.GetCenter()) > centerX;
    });

    const auto y1Pivot = std::partition(prims.begin(), xPivot, [&](const PrimRef& rhs) {
        return Second(rhs.GetCenter()) > centerY;
    });

    const auto y2Pivot = std::partition(
        xPivot, prims.end(), [&](const PrimRef& rhs) { return Second(rhs.GetCenter()) > centerY; });

    const std::ptrdiff_t x0 = y1Pivot - prims.begin();
    const std::ptrdiff_t x1 = xPivot - y1Pivot;
    const std::ptrdiff_t x2 = y2Pivot - xPivot;

    InteriorNode* const node = new InteriorNode;
    BoundingBox childBBox;
    const auto build = [&](std::ptrdiff_t start, std::ptrdiff_t count, std::uint32_t index) {
        node->children[index] = Build(prims.subspan(start, count), childBBox);
        node->childrenBoxes.Set(index, childBBox);
    };
    build(0, x0, 0);
    build(x0, x1, 1);
    build(xPivot - prims.begin(), x2, 2);
    build(y2Pivot - prims.begin(), prims.end() - y2Pivot, 3);
    return NodeRef{node};
}

NodeRef::NodeRef(const Leaf* leaf) : m_nodeRef{reinterpret_cast<std::uintptr_t>(leaf)}
{
    assert((m_nodeRef & kMask) == 0);
}

NodeRef::NodeRef(const InteriorNode* node)
    : m_nodeRef{reinterpret_cast<std::uintptr_t>(node) | kMask}
{
    // FIXME
    assert((reinterpret_cast<std::uintptr_t>(node) & kMask) == 1);
}

NodeRef::NodeRef() : m_nodeRef{} {}

gsl::span<const SimdTriangle> NodeRef::GetLeafPrimitives() const
{
    assert(GetKind() == kLeaf);
    return gsl::make_span(reinterpret_cast<const Leaf*>(GetPtr())->primitives);
}
