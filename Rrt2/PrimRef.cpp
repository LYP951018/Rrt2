#include "PrimRef.hpp"

PrimRef::PrimRef(const BoundingBox& bounds, std::uint32_t geomId, std::uint32_t primId)
{
    lower = bounds.corners[0];
    upper = bounds.corners[1];
    lower.m128_i32[3] = geomId;
    upper.m128_i32[3] = primId;
}

Float4 PrimRef::GetCenter() const { return Div(Add(lower, upper), MakeFloats(2.0f)); }

std::uint32_t PrimRef::GetPrimId() const { return bit_cast<std::uint32_t>(Fourth(upper)); }

std::uint32_t PrimRef::GetGeomId() const { return bit_cast<std::uint32_t>(Fourth(lower)); }
