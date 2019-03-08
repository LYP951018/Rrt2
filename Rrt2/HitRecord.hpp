#pragma once

#include "Vec3.hpp"

class GeometryBase;

struct HitRecord
{
    Vec3f position;
    Vec3f normal;
    float u, v;
    std::uint32_t primId;
    std::uint32_t geomId;
};