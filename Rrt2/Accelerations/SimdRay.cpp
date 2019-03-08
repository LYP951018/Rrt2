#include "SimdRay.hpp"
#include "../Ray.hpp"
#include "../Store.hpp"

void SimdRay::Fill(AlignedVec<SimdRay>& simdRays, gsl::span<Ray> plainRays)
{
    const std::uint32_t simdRequired =
        (static_cast<std::uint32_t>(plainRays.size()) + kMaxSimdWidth - 1) / kMaxSimdWidth;
    if (simdRequired == 0)
        return;
    const std::uint32_t chunkLoopCount = simdRequired - 1;
    simdRays.reserve(simdRequired);

    // TODO: duplicate code here
    for (std::uint32_t i = 0; i < chunkLoopCount; ++i)
    {
        Fill(simdRays, plainRays.data(), i * kMaxSimdWidth, kMaxSimdWidth);
    }
    const std::uint32_t raysLeft = plainRays.size() - chunkLoopCount * kMaxSimdWidth;
    Fill(simdRays, plainRays.data(), chunkLoopCount * kMaxSimdWidth, raysLeft);
}

void SimdRay::Fill(AlignedVec<SimdRay>& simdRays, const Ray* plainRays, std::uint32_t start,
                   std::uint32_t loopCount)
{
    SimdRay simdRay;
    for (std::uint32_t j = 0; j < loopCount; ++j)
    {
        simdRay.Set(j, plainRays[start + j]);
    }
    simdRay.Finalize();
    simdRays.push_back(simdRay);
}

void SimdRay::Set(std::uint32_t index, const Ray& ray)
{
    Store(index, origin, ray.origin);
    Store(index, speed, ray.speed);
}

void SimdRay::Finalize()
{
    for (std::uint32_t i = 0; i < 3; ++i)
    {
        // FIXME: div by zero?
        invSpeed[i] = Div(MakeFloats(1.0f), speed[i]);
        negSpeed[i] = Sub(ZeroFloats(), speed[i]);
    }
}
