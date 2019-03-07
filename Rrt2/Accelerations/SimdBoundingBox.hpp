#pragma once

#include "../Vec3.hpp"

class BoundingBox;

class SimdBoundingBox
{
  public:
    Vec3fPacked lower;
    Vec3fPacked upper;

    void Set(std::uint32_t i, const BoundingBox& bbox);
};