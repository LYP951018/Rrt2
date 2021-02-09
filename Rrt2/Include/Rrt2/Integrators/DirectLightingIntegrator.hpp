#pragma once

#include "IntegratorBase.hpp"
#include "../Sampling.hpp"
#include "../Spectrum.hpp"

namespace rrt
{
    class Camera;
    class Ray;

    class DirectLightingIntegrator // : public IntegratorBase
    {
      public:
        DirectLightingIntegrator(std::uint32_t samplesPerPixel);
        void Render(const Camera& camera, const Scene& scene); // override;

      private:
        RandomSampler m_randomSampler;
        PixelSampler m_pixelSampler;

        Spectrum Li(const Ray& ray, const Scene& scene);
    };

} // namespace rrt