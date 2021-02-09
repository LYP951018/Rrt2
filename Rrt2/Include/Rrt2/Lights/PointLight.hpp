#pragma once

#include "LightBase.hpp"
#include <glm/gtc/constants.hpp>

namespace rrt
{
    class PointLight : public LightBase
    {
      public:
        PointLight(const glm::mat3x4 lightToWorld, const Spectrum& powerPerSolidAngle)
            : LightBase{(int)LightKind::kDeltaPosition, lightToWorld},
              m_powerPerSolidAngle{powerPerSolidAngle},
              m_lightPos{LightToWorld(glm::zero<glm::vec3>())}
        {}

        Spectrum GetPower() const override;
        SampleLiResult SampleLi(const SurfaceInteraction& ref) const override;
        float GetPdfForDir(const SurfaceInteraction& start,
                           const Vec3f& dir) const override;

      private:
        // intensity
        const Spectrum m_powerPerSolidAngle;
        const Vec3f m_lightPos;
    };
} // namespace rrt
