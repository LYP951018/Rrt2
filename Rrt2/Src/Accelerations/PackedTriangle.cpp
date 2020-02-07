#include "PackedTriangle.hpp"
#include "Geometries/TriangleMesh.hpp"
#include "Scene.hpp"
#include "PrimRef.hpp"
#include "PackedRay.hpp"
#include "Ray.hpp"
#include "StorageLoadStore.hpp"
#include <algorithm>
#include <bit>

namespace rrt
{
    // FIXME
    Vec3f QuickSub(const Vec3f& lhs, const Vec3f& rhs)
    {
        return Vec3f{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
    }

    void PackedTriangleStorage::Set(std::uint32_t i, const SingleTriangle& triangle,
                             std::uint32_t primId, std::uint32_t geomId)
    {
        primIds[i] = primId;
        geomIds[i] = geomId;
        Store(i, v0, triangle.v0);
        const Vec3f _e1 = QuickSub(triangle.v1, triangle.v0);
        const Vec3f _e2 = QuickSub(triangle.v2, triangle.v0);
        Store(i, e1, _e1);
        Store(i, e2, _e2);
    }

    void PackedTriangleStorage::Fill(const PrimRefStorage* prims, std::uint32_t& start,
                              std::uint32_t end, const Scene* scene)
    {
        end = std::min(end, start + kMaxSimdWidth);
        for (std::uint32_t i = 0; i < end - start; ++i)
        {
            const PrimRefStorage& primRef = prims[start + i];
            const std::uint32_t geomId = primRef.geomId;
            const TriangleMesh& mesh =
                *static_cast<const TriangleMesh*>(scene->GetGeometryAt(geomId));
            const std::uint32_t primId = primRef.primId;
            Set(i, mesh.GetPrimitiveAt(primId), primId, geomId);
        }
    }

    std::optional<HitRecord> PackedTriangle::Hit(const PackedRay& packedRay,
                                                 const Ray& ray, float tMin,
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
        const Float4 minT =
            SelectMinElement(_mm_blendv_ps(GetInfinity(), t, uvMask));
        const Float4 maskOutGreater = Equal(minT, filtered);
        const std::uint8_t mask = Msbs(maskOutGreater);
        const int index = std::countr_zero(mask);
        const std::uint32_t geomId = geomIds[index];
        const std::uint32_t primId = primIds[index];
        HitRecord record;
        record.geomId = geomId;
        record.primId = primId;
        record.t = First(minT);
        SimdRay simdRay;
        ray.Load(simdRay);
        Store(simdRay.At(record.t), &record.position.x);
        return record;
    }

} // namespace rrt
