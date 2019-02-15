#include "Vec3.hpp"
#include <ostream>

Vec3 Vec3::FromVec(DirectX::XMVECTOR vec)
{
	using namespace DirectX;
	XMFLOAT4 f4;
	XMStoreFloat4(&f4, vec);
	return Vec3{ f4.x, f4.y, f4.z };
}

std::ostream& operator << (std::ostream& os, const Vec3& value)
{
	os << "[" << value.x << ", " << value.y << ", " << value.z << "]";
	return os;
}