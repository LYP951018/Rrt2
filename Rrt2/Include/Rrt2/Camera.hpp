#pragma once

#include <cstdint>
#include <memory>
#include <glm/matrix.hpp>
#include "Vec3.hpp"
#include <vector>
#include "Spectrum.hpp"
#include <span>

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

    class Film
    {
    public:
        // RGB
        using Pixel = glm::vec3;

        Film(const FilmSize& filmSize, std::uint32_t samplesPerPixel);
        std::span<const Pixel> GetPixels() const;
        const Pixel& GetPixelAt(const Vec2i& pos) const;
        void AddSample(const Vec2i& pos, const Spectrum& luminance);

        const FilmSize filmSize;

    private:
        std::vector<Pixel> m_pixels;
        const std::uint32_t m_samplesPerPixel;
    };

    // 最后写像素的时候，要从连续的坐标映射到最终要写入位置的离散坐标值
    // 用这个函数
    Vec2f FromContinuousToDiscrete(const Vec2f& cPos);

    class Camera
    {
      public:
        Camera(const PerspectiveParam& perspectiveParam,
               const glm::mat4& worldToCamera, const FilmSize& filmSize,
                std::uint32_t samplesPerPixel);

        // void GetRay(const Vec2fPacked& pixelPos, const Vec2fPacked&
        // thinLensPos, SimdRay& ray);
        Ray GetRay(const glm::vec2& continuousPos) const;

        const std::unique_ptr<Film> film;

      private:
        glm::mat4 m_cameraToWorld, m_worldToCamera, m_screenToRaster,
            m_cameraToScreen, m_screenToCamera;
        glm::mat4 m_rasterToWorld;
        glm::vec3 m_rayOrigin;
    };
} // namespace rrt
