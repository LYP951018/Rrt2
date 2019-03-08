#pragma once

#include "../Vec3.hpp"
#include "../AlignedVec.hpp"
#include <gsl/span>

class Ray;

class alignas(16) SimdRay
{
  public:
    Vec3fPacked origin;
    Vec3fPacked speed;
    Vec3fPacked invSpeed;
    Vec3fPacked negSpeed;

    static void Fill(AlignedVec<SimdRay>& simdRays, gsl::span<Ray> plainRays);

  private:
    static void Fill(AlignedVec<SimdRay>& simdRays, const Ray* rays, std::uint32_t start,
                     std::uint32_t loopCount);
    void Set(std::uint32_t index, const Ray& ray);
    void Finalize();
};