#pragma once

#include "Vec3.hpp"

namespace rrt
{
    class GeometryBase;

    struct SurfaceInteraction
    {
        Vec3f position;
        Vec3f normal;
        float u, v;
        float t;
        std::uint32_t primId;
        std::uint32_t geomId;
    };
}
