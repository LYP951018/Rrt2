#pragma once

#include "Vec3.hpp"

namespace rrt
{
    class Ray;

    class alignas(16) PackedRay
    {
      public:
        Vec3fPacked origin;
        Vec3fPacked speed;
        Vec3T<std::uint32_t> negSpeed;
        Vec3fPacked invSpeed;

        PackedRay(const Ray& ray);
    };
}
