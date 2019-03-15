#include "SimdTriangle.hpp"
#include "../Geometries/TriangleMesh.hpp"
#include "../Scene.hpp"
#include "../PrimRef.hpp"
#include "../Store.hpp"
#include "PackedRay.hpp"
#include "../Ray.hpp"
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
    const Vec3f _e2 = QuickSub(triangle.v2, triangle.v0);
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

std::optional<HitRecord> SimdTriangle::Hit(const PackedRay& packedRay, const Ray& ray, float tMin,
                                           float tMax) const
{
    const Vec3fPacked pVec = Cross3(packedRay.speed, e2);
    // FIXME: det == 0
    const PackedFloats det = Dot3(e1, pVec);
    const Vec3fPacked tVec = packedRay.origin - v0;
    const Vec3fPacked qVec = Cross3(tVec, e1);

    // TOOD: if tVec > det early ret?
    const PackedFloats invDet = Rcp(det);
    const PackedFloats u = Mul(Dot3(tVec, pVec), invDet);
    // FIXME: invDet is redundant here?
    const PackedFloats v = Mul(Dot3(packedRay.speed, qVec), invDet);
    const PackedFloats t = Mul(Dot3(e2, qVec), invDet);
    const auto inRange = [&](PackedFloats floats) {
        const PackedFloats biggerThanZero = Greater(floats, ZeroFloats());
        const PackedFloats lessThanOne = Less(floats, MakeFloats(1.0f));
        return And(lessThanOne, biggerThanZero);
    };
    const PackedFloats uvMask = And(inRange(u), inRange(v));
    const PackedFloats filtered = And(uvMask, t);
    // FIXME: if filtered == 0 return
    /*if (_mm_testz_si128(filtered, filtered) == 1)
    {
            return std::nullopt;
    }*/
    // get minimal t
    const Float4 minT = SelectMinElement(_mm_blendv_ps(GetInfinity(), t, uvMask));
    const Float4 maskOutGreater = Equal(minT, filtered);
    const int mask = Msbs(maskOutGreater);
    unsigned long index;
    _BitScanForward(&index, mask);
    const std::uint32_t geomId = geomIds.m128i_i32[index];
    const std::uint32_t primId = primIds.m128i_i32[index];
    HitRecord record;
    record.geomId = geomId;
    record.primId = primId;
    record.t = First(minT);
    Store(Add(ray.origin, Mul(ray.speed, minT)), &record.position.x);
    return record;
}
