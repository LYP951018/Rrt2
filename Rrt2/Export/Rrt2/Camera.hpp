#pragma once

#include <pcg_variants.h>
#include <cstdint>
#include <memory>
#include <glm/matrix.hpp>
#include "Vec3.hpp"
#include <vector>

namespace rrt
{
    class Ray;

    struct PerspectiveParam
    {
        float near, far;
        float fov;
    };

    struct FilmSize
    {
        std::uint32_t width, height;

        float GetAspectRatio() const
        {
            return static_cast<float>(height) / static_cast<float>(width);
        }
    };

    class Camera
    {
      public:
        Camera(const PerspectiveParam& perspectiveParam,
               const glm::mat4& worldToCamera, const FilmSize& filmSize);

        // void GetRay(const Vec2fPacked& pixelPos, const Vec2fPacked&
        // thinLensPos, SimdRay& ray);
        void GetRay(const glm::vec2& pixelPos, Ray& ray);
        void GenerateRays(pcg32_random_t& state, std::vector<Ray>& rays,
                          std::uint32_t samplesPerPixel);
      private:
        FilmSize m_filmSize;
        glm::mat4 m_cameraToWorld, m_worldToCamera, m_screenToRaster,
            m_cameraToScreen, m_screenToCamera;
        glm::mat4 m_rasterToWorld;
        glm::vec3 m_rayOrigin;
    };
} // namespace rrt
