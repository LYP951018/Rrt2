#pragma once

#include "AccelerationBase.hpp"

namespace rrt
{
    class TriangleBvh : public AccelerationBase
    {
      public:
        std::optional<HitRecord> Hit(const Ray& ray, float tMin, float tMax) override;
    };
}
