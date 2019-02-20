#pragma once

#include <gsl/span>
#include <pcg_variants.h>
#include "Vec3.hpp"

float Rand01(pcg32_random_t& state);

void GenRandSeq(pcg32_random_t& state, gsl::span<Vec2> seq);

void Jitter(pcg32_random_t& state, gsl::span<Vec2> seq);

void NRooks(pcg32_random_t& state, gsl::span<Vec2> seq);