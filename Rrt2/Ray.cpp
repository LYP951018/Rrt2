#include "Ray.hpp"
#include "Helpers.hpp"

// Vec3f Ray::GetPosAt(float t)
//{
//	return Vec3f{ m_origin.x + m_speed.x * t, m_origin.y + m_speed.y * t, m_origin.z + m_speed.z
//* t };
//}

void Ray::Finalize()
{
    invSpeed = Div(MakeFloats(1.0f), speed);
    negSpeed = Neg(speed);
}
