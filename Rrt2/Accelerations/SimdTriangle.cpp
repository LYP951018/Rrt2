#include "SimdTriangle.hpp"
#include "../Geometries/TriangleMesh.hpp"
#include "../Scene.hpp"
#include "../PrimRef.hpp"
#include "../Store.hpp"
#include <algorithm>

// FIXME
Vec3f QuickSub(const Vec3f& lhs, const Vec3f& rhs)
{
    return Vec3f{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

void SimdTriangle::Set(std::uint32_t i, const SingleTriangle& triangle, std::uint32_t primId,
                       std::uint32_t geomId)
{
    primIds.m128i_i32[i] = primId;
    geomIds.m128i_i32[i] = geomId;
    Store(i, v0, triangle.v0);
    const Vec3f _e1 = QuickSub(triangle.v1, triangle.v0);
    const Vec3f _e2 = QuickSub(triangle.v2, triangle.v1);
    Store(i, e1, _e1);
    Store(i, e2, _e2);
}

void SimdTriangle::Fill(const PrimRef* prims, std::uint32_t& start, std::uint32_t end,
                        const Scene* scene)
{
    end = std::min(end, start + kMaxSimdWidth);
    for (std::uint32_t i = 0; i < end - start; ++i)
    {
        const PrimRef& primRef = prims[start + i];
        const std::uint32_t geomId = primRef.GetGeomId();
        const TriangleMesh& mesh = *static_cast<const TriangleMesh*>(scene->GetGeometryAt(geomId));
        const std::uint32_t primId = primRef.GetPrimId();
        Set(i, mesh.GetPrimitiveAt(primId), primId, geomId);
    }
}
