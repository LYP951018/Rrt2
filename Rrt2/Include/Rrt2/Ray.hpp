#pragma once

#include <glm/vec3.hpp>
#include "Vec3.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Simd.hpp"
#include "MathBasics.hpp"

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

    class alignas(kSimdAlignment) Ray
    {
      public:
        glm::vec3 origin;
        float maxTime = kInf;
        glm::vec3 speed;
        float _padding2{};

        void Load(SimdRay& loadedRay) const
        {
            const int AllOneMask = -1;
            const Float4 mask =
                AsFloats(MakeInts(AllOneMask, AllOneMask, AllOneMask, 0));
            loadedRay.origin =
                And(mask, FloatsFromAlignedMemory(glm::value_ptr(origin)));
            loadedRay.speed = FloatsFromAlignedMemory(glm::value_ptr(speed));
        }
    };
}
