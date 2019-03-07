#pragma once

#include <cstdint>
#include "Vec3.hpp"

void Store(std::uint32_t i, Vec3fPacked& packed, const Vec3f& vec3);
void Store(std::uint32_t i, Vec3fPacked& packed, const PackedFloats& vec3);
