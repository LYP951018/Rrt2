#pragma once

#include "Vec3.hpp"

class Ray
{
  public:
    PackedFloats origin;
    PackedFloats speed;
    PackedFloats invSpeed;
    PackedFloats negSpeed;

    void Finalize();
};