#include <Rrt2/Lights/PointLight.hpp>
#include <Rrt2/MathBasics.hpp>
#include <glm/gtx/norm.hpp>

namespace rrt
{
    Spectrum PointLight::GetPower() const { return 4 * kPi * m_powerPerSolidAngle; }

    auto PointLight::SampleLi(const SurfaceInteraction& ref) const -> SampleLiResult
    {
        SampleLiResult result{};
        const float distance2 = glm::distance2(Vec3f{ref.position}, m_lightPos);
        result.visibilityTester = VisibilityTester{&m_lightPos, &ref};
        result.illuminance = m_powerPerSolidAngle / distance2;
        result.pdf = 1.0f;
        result.wiWorld = glm::normalize(glm::vec3{ref.position} - m_lightPos);
        return result;
    }

    float PointLight::GetPdfForDir(const SurfaceInteraction& start, const Vec3f& dir) const
    {
        return 0.0f;
    }
} // namespace rrt
