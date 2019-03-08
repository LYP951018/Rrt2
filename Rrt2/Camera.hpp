#pragma once

#include <pcg_variants.h>
#include <cstdint>
#include <memory>
#include "Vec3.hpp"
#include "AlignedVec.hpp"

class Ray;
class SimdRay;
class CameraData;

class Camera
{
  public:
    Camera(std::uint32_t filmWidth, std::uint32_t filmHeight, float radius, float distance,
           const Vec3f& center);

    void SetLookAt(const Vec3f& eye, const Vec3f& focus, const Vec3f& up);
    // void GetRay(const Vec2fPacked& pixelPos, const Vec2fPacked& thinLensPos, SimdRay& ray);
    void GetRay(const Vec2f& pixelPos, const Vec2f& thinLensPos, Ray& ray);
    void GenerateRays(pcg32_random_t& state, AlignedVec<Ray>& rays, std::uint32_t samplesPerPixel);

  private:
    std::uint32_t m_filmWidth, m_filmHeight;
    float m_radius;
    float m_distance;
    std::unique_ptr<CameraData> m_data;
};
