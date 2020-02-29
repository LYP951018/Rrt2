#pragma once

#include <cstddef>
#include "Simd.hpp"
#include <glm/vec4.hpp>

namespace rrt
{
    class Ray;
    class BoundingBox;

    class alignas(kSimdAlignment) BoundingBoxStorage
    {
      public:
        glm::vec4 corners[2];

        BoundingBox Load() const;
    };

    class alignas(kSimdAlignment) BoundingBox
    {
      public:
        Float4 corners[2];

        Float4 GetCenter() const
        {
            return Mul(Add(corners[0], corners[1]), MakeFloats(0.5f));
        }

        Float4 GetSize() const { return Sub(corners[1], corners[0]); }
        void VECTORCALL Extend(BoundingBox bbox);
        void VECTORCALL Extend(Float4 point);
        float GetHalfArea() const;
        static BoundingBox CreateInvalidBBox()
        {
            return BoundingBox{.corners = {GetInfinity(), GetNegInfinity()}};
        }

        void StoreTo(BoundingBoxStorage& storage) const;

      private:
        enum : std::size_t
        {
            kMinIndex,
            kMaxIndex
        };
    };
}
