#include "Store.hpp"

void Store(std::uint32_t i, Vec3fPacked& packed, const Vec3f& vec3)
{
    for (std::uint32_t j = 0; j < 3; ++j)
    {
        packed[j].m128_f32[i] = (&vec3.x)[j];
    }
}

void Store(std::uint32_t i, Vec3fPacked& packed, const PackedFloats& vec3)
{
    for (std::uint32_t j = 0; j < 3; ++j)
    {
        packed[j].m128_f32[i] = vec3.m128_f32[j];
    }
}
