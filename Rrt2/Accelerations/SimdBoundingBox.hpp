#pragma once

#include "../Vec3.hpp"

class BoundingBox;
class PackedRay;

class SimdBoundingBox
{
  public:
    Vec3fPacked lower;
    Vec3fPacked upper;

	std::uint32_t Hit(const PackedRay& ray, float tMin, float tMax) const;
    void Set(std::uint32_t i, const BoundingBox& bbox);

private:
	const Vec3fPacked& Get(std::size_t n) const;
};