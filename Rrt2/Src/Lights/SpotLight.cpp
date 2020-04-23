#include <Rrt2/Lights/SpotLight.hpp>
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include <Rrt2/MathBasics.hpp>

namespace rrt
{
    SpotLight::SpotLight(const Vec3f& lightPos, float totalWidthAngle,
                         float fallOffWidthAngle,
                         Spectrum maxPowerPerSolidAngle)
        : LightBase{(int)LightKind::kDeltaPosition},
          m_cosTotalWidth{std::cos(totalWidthAngle)},
          m_cosFallOffWidth{std::cos(fallOffWidthAngle)},
          m_lightPos{lightPos},
          m_maxPowerPerSolidAngle{maxPowerPerSolidAngle}
    {}

    Spectrum SpotLight::SampleLi(const SurfaceInteraction& interaction,
                                 VisibilityTester& visibilityTester) const
    {
        const float fallOff = FallOff(interaction.position);
        const float distance2 = glm::length2(interaction.position - m_lightPos);
        return fallOff * m_maxPowerPerSolidAngle / distance2;
    }

    // cone 的 solid angle 是 2 pi (1 - cos\theta)
    Spectrum SpotLight::GetPower() const
    {
        return 2 * kPi * (1 - 0.5f * (m_cosFallOffWidth + m_cosTotalWidth)) *
               m_maxPowerPerSolidAngle;
    }

    float SpotLight::FallOff(const Vec3f& p) const
    {
        const Vec3f toVec = glm::normalize(p);
        const float cosTheta = glm::dot(toVec, glm::vec3(0.0f, -1.0f, 0.0f));
        if (cosTheta < m_cosTotalWidth)
            return 0.0f;
        if (cosTheta > m_cosFallOffWidth)
            return 1.0f;
        const float delta = (m_cosFallOffWidth - cosTheta) /
                            (m_cosFallOffWidth - m_cosTotalWidth);
        return (delta * delta) * (delta * delta);
    }
} // namespace rrt