#include "TestHelpers.hpp"

bool operator==(const PackedFloats& lhs, const PackedFloats& rhs)
{
    return Msbs(Equal(lhs, rhs)) == 0xF;
}

bool operator==(const Vec3f& lhs, const Vec3f& rhs)
{
    return Approx{lhs.x} == rhs.x && Approx{lhs.y} == rhs.y && Approx{lhs.z} == rhs.z;
}
