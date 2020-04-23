#include <Rrt2/Lights/LightBase.hpp>

namespace rrt
{
    LightBase::~LightBase() {}

    Vec3f LightBase::WorldToLightSpace(const Vec3f& p) const
    {
        // 目前 light 没有 local space
        return p;
    }
} // namespace rrt
