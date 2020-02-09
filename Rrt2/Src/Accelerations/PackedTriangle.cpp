#include "Rrt2/Accelerations/PackedTriangle.hpp"
#include "Rrt2/Geometries/TriangleMesh.hpp"
#include "Rrt2/Scene.hpp"
#include "Rrt2/PrimRef.hpp"
#include "Rrt2/Accelerations/PackedRay.hpp"
#include "Rrt2/Ray.hpp"
#include "Rrt2/StorageLoadStore.hpp"
#include <algorithm>
#include <bit>

namespace rrt
{
    void PackedTriangleStorage::Set(std::uint32_t i,
                                    const SingleTriangle& triangle,
                                    std::uint32_t primId, std::uint32_t geomId)
    {
        primIds[i] = primId;
        geomIds[i] = geomId;
        Store(i, v0, triangle.v0);
        const Vec3f _e1 = triangle.v1 - triangle.v0;
        const Vec3f _e2 = triangle.v2 - triangle.v0;
        Store(i, e1, _e1);
        Store(i, e2, _e2);
    }

    void PackedTriangleStorage::Fill(const PrimRefStorage* prims,
                                     std::uint32_t& start, std::uint32_t end,
                                     const Scene* scene)
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
        if (IsZero(filtered))
        {
            return std::nullopt;
        }
        // filtered: 0 0 F F
        // get minimal t
        // INF, INF, t1, t2
        // 从上面选出最小的 t2
        // minT: t2 t2 t2 t2
        const Float4 minT =
            SelectMinElement(_mm_blendv_ps(GetInfinity(), t, uvMask));
        // 下面的步骤为了找出最小的 t 的索引
        // (t2, t2, t2, t2) == (INF, INF, t1, t2) => (0, 0, 0, T)
        const Float4 maskOutGreater = Equal(minT, filtered);
        // 0001
        const std::uint8_t mask = Msbs(maskOutGreater);
        // index = 0
        const int index = std::countr_zero(mask);
        assert(index != sizeof(mask) * 8);
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
