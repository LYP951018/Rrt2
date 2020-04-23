#include <Rrt2/HitRecord.hpp>
#include <Rrt2/Ray.hpp>

namespace rrt
{
    Ray SurfaceInteraction::SpawnRayTo(const Vec3f& dst) const
    {
        const Vec3f dist = dst - position;
        return Ray{position, dist, 1.0f};
    }
} // namespace rrt
