#include "Rrt2/Camera.hpp"
#include "Rrt2/Sampling.hpp"
#include "Rrt2/Ray.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace rrt
{
    Camera::Camera(const PerspectiveParam& perspectiveParam,
                   const glm::mat4& worldToCamera, const FilmSize& filmSize)
        : m_worldToCamera{worldToCamera}, m_filmSize{filmSize}
    {
        const float aspectRatio = filmSize.GetAspectRatio();
        m_cameraToScreen = glm::perspectiveLH<float>(
            perspectiveParam.fov, aspectRatio, perspectiveParam.near,
            perspectiveParam.far);
        m_cameraToWorld = glm::inverse(m_worldToCamera);
        m_screenToCamera = glm::inverse(m_cameraToScreen);
        m_screenToRaster =
            glm::scale(glm::vec3(static_cast<float>(filmSize.width),
                                 -static_cast<float>(filmSize.height), 1.0f)) *
            glm::translate<float>(glm::vec3(0.5f, -0.5f, 0.0f));
        m_rasterToWorld = glm::inverse(m_screenToRaster * m_cameraToScreen * m_worldToCamera);
        m_rayOrigin = (m_cameraToWorld * glm::vec4{glm::vec3{0.0f}, 1.0f});
    }

    void Camera::GetRay(const glm::vec2& pixelPos, Ray& ray)
    {
        ray.origin = m_rayOrigin;
        const glm::vec4 rasterPos4D = glm::vec4(pixelPos, 1.0f, 1.0f);
        const glm::vec3 worldSpaceEndPoint =
            glm::vec3(m_rasterToWorld * rasterPos4D);
        const glm::vec3 direction =
            glm::normalize(worldSpaceEndPoint - m_rayOrigin);
        ray.speed = direction;
    }

    void Camera::GenerateRays(pcg32_random_t& state, std::vector<Ray>& rays,
                              std::uint32_t samplesPerPixel)
    {
        auto [filmWidth, filmHeight] = m_filmSize;
        const std::uint32_t samplesCount =
            filmWidth * filmHeight * samplesPerPixel;
        const glm::vec2 filmSizeVec2 = glm::vec2(
            static_cast<float>(filmWidth), static_cast<float>(filmHeight));
        rays.reserve(samplesCount);
        std::vector<glm::vec2> pixelPoses(samplesPerPixel);
        std::uint32_t samplesPerPixelWidth =
            static_cast<std::uint32_t>(std::sqrt(samplesPerPixel));
        for (std::uint32_t w = 0; w < filmWidth; ++w)
        {
            for (std::uint32_t h = 0; h < filmHeight; ++h)
            {
                Jitter(state, gsl::make_span(pixelPoses));
                for (std::uint32_t i = 0; i < samplesPerPixel; ++i)
                {
                    Ray ray;
                    glm::vec2& pixelPos = pixelPoses[i];
                    pixelPos +=
                        glm::vec2(static_cast<float>(w), static_cast<float>(h));
                    GetRay(pixelPos, ray);
                    rays.push_back(ray);
                }
            }
        }
    }
} // namespace rrt
