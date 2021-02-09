#include <Rrt2/Lights/LightBase.hpp>
#include <Rrt2/Ray.hpp>
#include <Rrt2/Scene.hpp>

namespace rrt
{
    LightBase::~LightBase() {}

    Vec3f LightBase::WorldToLight(const Vec3f& p) const
    {
        return glm::inverse(m_lightToWorld) * p;
    }

    Vec3f LightBase::LightToWorld(const Vec3f& p) const
    {
        return m_lightToWorld * p;
    }

    bool VisibilityTester::Unoccluded(const Scene& scene) const
    {
        const Ray testRay = p1->SpawnRayTo(*p0);
        return scene.Hit(testRay);
    }
} // namespace rrt
