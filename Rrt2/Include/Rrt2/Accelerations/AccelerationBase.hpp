#pragma once

#include <optional>
#include "../HitRecord.hpp"

namespace rrt
{
    class Ray;

    class AccelerationBase
    {
      public:
        virtual std::optional<SurfaceInteraction> Trace(const Ray& ray) = 0;
        virtual bool Hit(const Ray& ray) = 0;
        virtual ~AccelerationBase();
        virtual void Build() = 0;
    };
}
