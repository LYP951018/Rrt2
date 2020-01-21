#pragma once

#include <gsl/span>
#include <pcg_variants.h>
#include "Vec3.hpp"

float Rand01(pcg32_random_t& state);

void GenRandSeq(pcg32_random_t& state, gsl::span<Vec2f> seq);

void Jitter(pcg32_random_t& state, gsl::span<Vec2f> seq);

void NRooks(pcg32_random_t& state, gsl::span<Vec2f> seq);

void BoxFilter(gsl::span<Vec2f> seq);