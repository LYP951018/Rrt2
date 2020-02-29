#pragma once

#include "Simd.hpp"
#include "BoundingBox.hpp"
#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace rrt
{
    class alignas(kSimdAlignment) PrimRef
    {
      public:
        Float4 lower; // lower bounds and geomid
        Float4 upper; // upper bounds and primid

        Float4 GetCenter() const;
    };

    class alignas(kSimdAlignment) PrimRefStorage
    {
      public:
        glm::vec3 lower;
        std::uint32_t geomId;
        glm::vec3 upper;
        std::uint32_t primId;
        PrimRefStorage(const BoundingBox& bounds, std::uint32_t geomId,
                       std::uint32_t primId);

        //        std::uint32_t GetPrimId() const;
        //        std::uint32_t GetGeomId() const;

        inline PrimRef Load() const
        {
            return PrimRef{
                .lower = FloatsFromAlignedMemory(glm::value_ptr(lower)),
                .upper = FloatsFromAlignedMemory(glm::value_ptr(upper))};
        }
    };

} // namespace rrt
