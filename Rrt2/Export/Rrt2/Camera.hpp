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
    Camera(const XMMATRIX& cameraToWorld, const XMMATRIX& cameraToScreen,  float distance);

    void SetLookAt(const Vec3f& eye, const Vec3f& focus, const Vec3f& up);
    // void GetRay(const Vec2fPacked& pixelPos, const Vec2fPacked& thinLensPos, SimdRay& ray);
    void GetRay(const Vec2f& pixelPos, Ray& ray);
    void GenerateRays(pcg32_random_t& state, AlignedVec<Ray>& rays, std::uint32_t samplesPerPixel);
    ~Camera();

  private:
    std::uint32_t m_filmWidth, m_filmHeight;
    float m_distance;
    std::unique_ptr<CameraData> m_data;
};
