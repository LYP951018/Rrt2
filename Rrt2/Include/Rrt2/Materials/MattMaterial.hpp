#pragma once

#include "../MaterialBase.hpp"
#include "../TextureBase.hpp"
#include "../Spectrum.hpp"

namespace rrt
{
    class DiffuseOnlyMaterial : public MaterialBase
    {
      public:
        DiffuseOnlyMaterial(std::shared_ptr<const TextureBase<Spectrum>> kd);
        void ComputeScatteringFunctions(SurfaceInteraction& interaction) override;

      private:
        std::shared_ptr<const TextureBase<Spectrum>> m_kd;
    };

} // namespace rrt