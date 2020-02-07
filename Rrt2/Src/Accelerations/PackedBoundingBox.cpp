﻿#include "PackedBoundingBox.hpp"
#include "BoundingBox.hpp"
#include "PackedRay.hpp"
#include "StorageLoadStore.hpp"
#include <algorithm>

namespace rrt
{
    // https://tavianator.com/fast-branchless-raybounding-box-intersections-part-2-nans/
    std::uint32_t PackedBoundingBox::Hit(const PackedRay& ray, float tMin, float tMax) const
    {
        //第一个索引：在平行的两个面中选一个
        //第二个索引：x, y, z
        //这里不处理 origin 和 AABB 某个顶点的某个维度相等，并且这个维度的 speed 为 0 的问题：
        // speed 为 0，invSpeed 为 +-∞，origin 的某个维度（x, y, z）与 AABB 相减后，为 0，0 * ∞ ==
        // NaN。
        PackedFloats t0Min = GetNegInfinity();
        PackedFloats t1Max = GetInfinity();
        for (unsigned i = 0; i < 3; ++i)
        {
            const PackedFloats& lower = Get(ray.negSpeed[i])[i];
            const PackedFloats& upper = Get(1 - ray.negSpeed[i])[i];
            const PackedFloats t0 = Mul(Sub(lower, ray.origin[i]), ray.invSpeed[i]);
            const PackedFloats t1 = Mul(Sub(upper, ray.origin[i]), ray.invSpeed[i]);
            t0Min = Max(t0Min, t0);
            t1Max = Min(t1Max, t1);
        }
        t0Min = Max(t0Min, MakeFloats(tMin));
        t1Max = Min(t1Max, MakeFloats(tMax));
        const int mask = Msbs(Sub(t0Min, t1Max));
        return static_cast<std::uint32_t>(mask);
    }

    void PackedBoundingBoxStorage::Set(std::uint32_t i, const BoundingBoxStorage& bbox)
    {
        Store(i, lower, glm::vec3{bbox.corners[0]});
        Store(i, upper, glm::vec3{bbox.corners[1]});
    }

    const Vec3fPacked& PackedBoundingBox::Get(std::size_t n) const
    {
        assert(n == 0 || n == 1);
        return (&lower)[n];
    }

}