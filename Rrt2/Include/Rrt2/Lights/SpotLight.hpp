#pragma once

#include "LightBase.hpp"

namespace rrt
{
    class SpotLight : public LightBase
    {
      public:
        SpotLight(const Vec3f& lightPos, float totalWidthAngle,
                  float fallOffWidthAngle, Spectrum maxPowerPerSolidAngle);

        Spectrum SampleLi(const SurfaceInteraction& interaction,
                          VisibilityTester& visibilityTester) const override;
        Spectrum GetPower() const override;

      private:
        const float m_cosTotalWidth, m_cosFallOffWidth;
        // world space light pos
        const Vec3f m_lightPos;
        Spectrum m_maxPowerPerSolidAngle;

        float FallOff(const Vec3f& p) const;
    };
} // namespace rrt