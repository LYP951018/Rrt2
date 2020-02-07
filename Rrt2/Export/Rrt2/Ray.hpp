#pragma once

#include <glm/vec3.hpp>
#include <Vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Simd.hpp"

namespace rrt
{
    struct alignas(kSimdAlignment) SimdRay
    {
        Float4 origin;
        Float4 speed;

        inline Float4 At(float t) const
        {
            // TODO: FMA
            return Add(origin, Mul(speed, MakeFloats(t)));
        }
    };

    struct alignas(kSimdAlignment) Ray
    {
        glm::vec3 origin;
        float _padding1;
        glm::vec3 speed;
        float _padding2;

        inline void Load(SimdRay& loadedRay) const
        {
            loadedRay.origin = FloatsFromAlignedMemory(glm::value_ptr(origin));
            loadedRay.speed = FloatsFromAlignedMemory(glm::value_ptr(speed));
        }
    };
}
