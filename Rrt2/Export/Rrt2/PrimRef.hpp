#pragma once

#include "Simd.hpp"
#include "Helpers.hpp"
#include "BoundingBox.hpp"
#include <glm/vec3.hpp>

namespace rrt
{
    class alignas(kSimdAlignment) LoadedPrimRef
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
        PrimRefStorage(const BoundingBox& bounds, std::uint32_t geomId, std::uint32_t primId);

//        std::uint32_t GetPrimId() const;
//        std::uint32_t GetGeomId() const;

        inline LoadedPrimRef Load() const {}

    };


}
