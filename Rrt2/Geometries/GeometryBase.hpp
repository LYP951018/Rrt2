#pragma once

#include <optional>
#include "../HitRecord.hpp"

class Ray;
class BoundingBox;

class GeometryBase
{
public:
	virtual std::optional<HitRecord> Hit(const Ray& ray, float tMin, float tMax) = 0;
	virtual const BoundingBox& GetBoundingBox() const = 0;
	virtual ~GeometryBase();
};