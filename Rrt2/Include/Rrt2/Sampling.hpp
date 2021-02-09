#pragma once

#include <span>
#include <pcg_variants.h>
#include <glm/vec2.hpp>
#include <vector>
#include "Vec3.hpp"

namespace rrt
{
    // float Rand01(pcg32_random_t& state);

    // void GenRandSeq(pcg32_random_t& state, gsl::span<Vec2f> seq);

    // void Jitter(pcg32_random_t& state, gsl::span<Vec2f> seq);

    // void NRooks(pcg32_random_t& state, gsl::span<Vec2f> seq);

    // void BoxFilter(gsl::span<Vec2f> seq);

    class RandomSampler
    {
      public:
        RandomSampler();
        float Rand1D();
        Vec2f Rand2D();

      private:
        pcg32_random_t m_rngState;
        const float m_maxFloatLTOne;

        std::uint32_t RandUInt32();
        float RandFloat();
    };

    class PixelSampler
    {
      public:
        PixelSampler(std::uint32_t samplesPerPixel);
        // continuous coordinates
        Vec2f GetCameraSample();
        std::span<const Vec2f> GetSampledPixels() const;
        void StartWithPixel(const Vec2i& cPos);

      private:
        const std::uint32_t m_samplesPerPixel;
        RandomSampler m_randomSampler;
        std::vector<Vec2f> m_sampledPixels;
        Vec2i m_currentPixel;

        void Jitter(std::span<Vec2f> seq, std::int32_t x, std::int32_t y);
    };
} // namespace rrt
