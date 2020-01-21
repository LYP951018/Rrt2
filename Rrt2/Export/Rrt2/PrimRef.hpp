#pragma once

#include "Simd.hpp"
#include "Helpers.hpp"
#include "BoundingBox.hpp"

class alignas(16) PrimRef
{
  public:
    Float4 lower; // lower bounds and geomid
    Float4 upper; // upper bounds and primid

    PrimRef(const BoundingBox& bounds, std::uint32_t geomId, std::uint32_t primId);
    Float4 GetCenter() const;

    std::uint32_t GetPrimId() const;
    std::uint32_t GetGeomId() const;
};