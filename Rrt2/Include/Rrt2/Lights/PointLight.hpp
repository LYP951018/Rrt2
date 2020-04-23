#pragma once

#include "LightBase.hpp"

namespace rrt
{
    class PointLight : public LightBase
    {
      public:
        PointLight(const Vec3f& lightPos, const Spectrum& powerPerSolidAngle)
            : LightBase{(int)LightKind::kDeltaPosition},
              m_powerPerSolidAngle{powerPerSolidAngle},
              m_lightPos{lightPos}
        {}

        Spectrum GetPower() const override;
        Spectrum SampleLi(const SurfaceInteraction& interaction,
                          VisibilityTester& visibilityTester) const override;

      private:
        const Spectrum m_powerPerSolidAngle;
        const Vec3f m_lightPos;
    };
} // namespace rrt
