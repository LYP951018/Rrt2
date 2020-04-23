#pragma once

#include "../Spectrum.hpp"
#include "../HitRecord.hpp"

namespace rrt
{
    enum class LightKind
    {
        kDeltaPosition = 1,
        kDeltaDirection = 2,
        kArea = 4,
        kInfinite = 8
    };

    class Scene;

    class VisibilityTester
    {
      public:
        const SurfaceInteraction p0, p1;
        bool Unoccluded(const Scene& scene) const;
    };

    class LightBase
    {
      public:
        LightBase(int _lightType)
            : lightType{_lightType}
        {}

        // 用于估算该灯光的输出功率
        // 输出大的可以倾斜计算资源多算一算
        virtual Spectrum GetPower() const = 0;
        virtual Spectrum
        SampleLi(const SurfaceInteraction& interaction,
                 VisibilityTester& visibilityTester) const = 0;
        virtual ~LightBase();
        Vec3f WorldToLightSpace(const Vec3f& p) const;

        const int lightType;
    };
} // namespace rrt