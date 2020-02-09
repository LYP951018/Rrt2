#include "Rrt2/Accelerations/PackedRay.hpp"
#include "Rrt2/Ray.hpp"

namespace rrt
{
    PackedRay::PackedRay(const Ray& ray)
    {
        origin = {
            MakeFloats(ray.origin[0]),
            MakeFloats(ray.origin[1]),
            MakeFloats(ray.origin[2]),
        };

        speed = {MakeFloats(ray.speed[0]), MakeFloats(ray.speed[1]),
                 MakeFloats(ray.speed[2])};

        negSpeed = {ray.speed[0] < 0.0f, ray.speed[1] < 0.0f,
                    ray.speed[2] < 0.0f};
        invSpeed = Rcp(speed);
    }
}
