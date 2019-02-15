#pragma once

#include "CommonDefs.hpp"

class GeometryBase;

struct HitRecord
{
	PointType position;
	PointType normal;
	float u, v;
};