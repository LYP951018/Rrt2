#pragma once

#include "HitRecord.hpp"

namespace rrt
{
    class MaterialBase
    {
      public:
        virtual void
        ComputeScatteringFunctions(SurfaceInteraction& surfaceInteraction) const = 0;
        virtual ~MaterialBase();
    };
} // namespace rrt
