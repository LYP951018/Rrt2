#pragma once

#include <cstddef>
#include "Simd.hpp"
#include <glm/vec4.hpp>

namespace rrt
{
    class Ray;

    class alignas(kSimdAlignment) BoundingBoxStorage
    {
      public:
        glm::vec4 corners[2];
    };

    class alignas(kSimdAlignment) BoundingBox
    {
      public:
        Float4 corners[2];

        Float4 GetCenter() const
        {
            return Mul(Add(corners[0], corners[1]), MakeFloats(0.5f));
        }


      private:
        enum : std::size_t
        {
            kMinIndex,
            kMaxIndex
        };
    };
}
