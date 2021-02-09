#include "HitRecord.hpp"
#include "Ray.hpp"

namespace rrt
{
    Ray SurfaceInteraction::SpawnRay(const Vec3f& dir) const
    {
        // 暂时不处理浮点误差（参考 pbrt：OffsetRayOrigin）
        return Ray{position, dir};
    }
}
