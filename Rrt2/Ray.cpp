#include "Ray.hpp"
#include "Helpers.hpp"

PointType Ray::GetPosAt(float t) 
{ 
	return PointType{ m_origin.x + m_speed.x * t, m_origin.y + m_speed.y * t, m_origin.z + m_speed.z * t };
}