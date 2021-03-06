﻿#include "Rrt2/Sampling.hpp"
#include <cassert>
#include <algorithm>
#include <cmath>

namespace rrt
{
    float Rand01(pcg32_random_t& state) { return std::ldexp(pcg32_random_r(&state), -32); }

    void GenRandSeq(pcg32_random_t& state, gsl::span<glm::vec2> seq)
    {
        for (glm::vec2& p : seq)
        {
            p.x = Rand01(state);
            p.y = Rand01(state);
        }
    }

    void Jitter(pcg32_random_t& state, gsl::span<glm::vec2> seq)
    {
        assert(seq.size() <= UINT32_MAX);
        const std::uint32_t sampleCountSqrt = static_cast<std::uint32_t>(
            std::sqrt(static_cast<float>(static_cast<std::uint32_t>(seq.size()))));
        const float unitLength = 1.0f / static_cast<float>(sampleCountSqrt);
        for (std::uint32_t i = 0; i < sampleCountSqrt; ++i)
        {
            for (std::uint32_t j = 0; j < sampleCountSqrt; ++j)
            {
                glm::vec2& p = seq[j * sampleCountSqrt + i];
                p.x = (static_cast<float>(i) + Rand01(state)) * unitLength;
                p.y = (static_cast<float>(j) + Rand01(state)) * unitLength;
            }
        }
    }

    void NRooks(pcg32_random_t& state, gsl::span<glm::vec2> seq)
    {
        assert(seq.size() <= UINT32_MAX);
        const std::uint32_t sampleCount = static_cast<std::uint32_t>(seq.size());
        assert(sampleCount > 1);
        const float unitLength = 1.0f / static_cast<float>(sampleCount);

        for (std::uint32_t i = 0; i < sampleCount; ++i)
        {
            Vec2f& p = seq[i];
            p.x = (static_cast<float>(i) + Rand01(state)) * unitLength;
            p.y = (static_cast<float>(i) + Rand01(state)) * unitLength;
        }

        for (std::uint32_t i = 0; i < sampleCount - 1; ++i)
        {
            const std::uint32_t destIndex = Rand01(state) * (sampleCount - 1);
            std::swap(seq[i].x, seq[destIndex].x);
        }
    }

    void BoxFilter(gsl::span<glm::vec2> seq)
    {
        for (glm::vec2& vec2 : seq)
        {
            vec2.x -= 0.5f;
            vec2.y -= 0.5f;
        }
    }
}

