#pragma once

#include "../Spectrum.hpp"
#include "../HitRecord.hpp"
#include <glm/mat3x4.hpp>

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

    struct SampleLiResult
    {
        // cd/m^2
        Spectrum illuminance;
        Vec3f wiWorld;
        float pdf;
        VisibilityTester visibilityTester;
    };

    class VisibilityTester
    {
      public:
        const Vec3f* p0;
        const SurfaceInteraction* p1;
        // p0 到 p1 是否有遮挡？
        bool Unoccluded(const Scene& scene) const;
    };

    class LightBase
    {
      public:
        LightBase(int _lightType, const glm::mat3x4 lightToWorld)
            : lightType{_lightType}, m_lightToWorld{lightToWorld}
        {}

        // 用于估算该灯光的输出功率
        // 输出大的可以倾斜计算资源多算一算
        virtual Spectrum GetPower() const = 0;

        virtual SampleLiResult SampleLi(const SurfaceInteraction& ref) const = 0;
        virtual float GetPdfForDir(const SurfaceInteraction& start, const Vec3f& wi) const = 0;
        virtual ~LightBase();
        Vec3f WorldToLight(const Vec3f& p) const;
        Vec3f LightToWorld(const Vec3f& p) const;

        const int lightType;

      private:
        const glm::mat3x4 m_lightToWorld;
    };
} // namespace rrt