#include "Rrt2/Sampling.hpp"
#include <cassert>
#include <algorithm>
#include <cmath>

namespace rrt
{

    /*float Rand01(pcg32_random_t& state) { return std::ldexp(pcg32_random_r(&state),
    -32); }

    void GenRandSeq(pcg32_random_t& state, gsl::span<glm::vec2> seq)
    {
        for (glm::vec2& p : seq)
        {
            p.x = Rand01(state);
            p.y = Rand01(state);
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
    }*/

    RandomSampler::RandomSampler() : m_maxFloatLTOne{std::nextafter(1.0f, 0.0f)} {}

    float RandomSampler::Rand1D() { return RandFloat(); }

    Vec2f RandomSampler::Rand2D() { return Vec2f{RandFloat(), RandFloat()}; }

    std::uint32_t RandomSampler::RandUInt32() { return pcg32_random_r(&m_rngState); }

    float RandomSampler::RandFloat()
    {
        const std::uint32_t randUInt32 = RandUInt32();
        return std::min(static_cast<float>(randUInt32) * 0x1.p-32f, m_maxFloatLTOne);
    }

    PixelSampler::PixelSampler(std::uint32_t samplesPerPixel)
        : m_samplesPerPixel{samplesPerPixel}
    {
        m_sampledPixels.resize(samplesPerPixel);
    }

    Vec2f PixelSampler::GetCameraSample()
    {
        return Vec2f{ m_currentPixel } + m_randomSampler.Rand2D();
    }

    std::span<const Vec2f> PixelSampler::GetSampledPixels() const
    {
        return std::span{ m_sampledPixels };
    }

    void PixelSampler::StartWithPixel(const Vec2i& cPos)
    {
        m_currentPixel = cPos;
        Jitter(std::span{m_sampledPixels}, m_samplesPerPixel, m_samplesPerPixel);
    }

    void PixelSampler::Jitter(std::span<Vec2f> seq, std::int32_t x, std::int32_t y)
    {
        assert(seq.size() <= UINT32_MAX);
        std::size_t pos = 0;
        const float dx = 1.0f / static_cast<float>(x);
        const float dy = 1.0f / static_cast<float>(y);
        for (std::uint32_t i = 0; i < x; ++i)
        {
            for (std::uint32_t j = 0; j < y; ++j)
            {
                Vec2f p;
                p.x = (static_cast<float>(i) + m_randomSampler.Rand1D()) * dx;
                p.y = (static_cast<float>(j) + m_randomSampler.Rand1D()) * dy;
                seq[pos] = p;
                ++pos;
            }
        }
    }

} // namespace rrt
