#include "PackedRay.hpp"
#include "../Ray.hpp"

PackedRay::PackedRay(const Ray& ray)
{
    origin = {
        MakeFloats(ray.origin.m128_f32[0]),
        MakeFloats(ray.origin.m128_f32[1]),
        MakeFloats(ray.origin.m128_f32[2]),
    };

    speed = {MakeFloats(ray.speed.m128_f32[0]), MakeFloats(ray.speed.m128_f32[1]),
             MakeFloats(ray.speed.m128_f32[2])};

    negSpeed = {ray.speed.m128_f32[0] < 0.0f, ray.speed.m128_f32[1] < 0.0f,
                ray.speed.m128_f32[2] < 0.0f};
    invSpeed = Rcp(speed);
}
