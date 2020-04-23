#include <Rrt2/Lights/PointLight.hpp>
#include <Rrt2/MathBasics.hpp>
#include <glm/gtx/norm.hpp>

namespace rrt
{
    Spectrum PointLight::GetPower() const
    {
        return 4 * kPi * m_powerPerSolidAngle;
    }

    Spectrum PointLight::SampleLi(const SurfaceInteraction& interaction,
                                  VisibilityTester& visibilityTester) const
    {
        const float distance2 =
            glm::distance2(interaction.position, m_lightPos);
        visibilityTester = VisibilityTester{m_lightPos, interaction.position};
        return m_powerPerSolidAngle / distance2;
    }
} // namespace rrt
