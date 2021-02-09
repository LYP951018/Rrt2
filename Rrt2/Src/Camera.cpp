#include "Rrt2/Camera.hpp"
#include "Rrt2/Sampling.hpp"
#include "Rrt2/Ray.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace rrt
{
    Camera::Camera(const PerspectiveParam& perspectiveParam,
                   const glm::mat4& worldToCamera, const FilmSize& filmSize,
                   std::uint32_t samplesPerPixel)
        : m_worldToCamera{worldToCamera},
          film{std::make_unique<Film>(filmSize, samplesPerPixel)}
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

    Ray Camera::GetRay(const glm::vec2& pixelPos) const
    {
        const glm::vec4 rasterPos4D = glm::vec4(pixelPos, 0.0f, 1.0f);
        const glm::vec4 invPersp = m_rasterToWorld * rasterPos4D;
        const glm::vec3 worldSpaceEndPoint = glm::vec3{invPersp / invPersp.w};
        const glm::vec3 direction =
            glm::normalize(worldSpaceEndPoint - m_rayOrigin);
        return Ray{
            .origin = m_rayOrigin,
            .speed = direction
        };
    }

    Vec2f FromContinuousToDiscrete(const Vec2f& cPos)
    {
        const auto convert = [](float f) { return std::floor(f + 0.5f); };
        return Vec2f{convert(cPos.x), convert(cPos.y)};
    }

    void Film::AddSample(const Vec2i& pos, const Spectrum& luminance)
    {
    }
} // namespace rrt
