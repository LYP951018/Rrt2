#pragma once

#include <optional>
#include "HitRecord.hpp"

namespace rrt
{
    class Ray;

    class AccelerationBase
    {
      public:
        virtual std::optional<HitRecord> Hit(const Ray& ray, float tMin, float tMax) = 0;
        virtual ~AccelerationBase();
        virtual void Build() = 0;
    };
}
