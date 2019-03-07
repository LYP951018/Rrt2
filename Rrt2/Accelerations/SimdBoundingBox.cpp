#include "SimdBoundingBox.hpp"
#include "../BoundingBox.hpp"
#include "../Store.hpp"

void SimdBoundingBox::Set(std::uint32_t i, const BoundingBox& bbox)
{
    Store(i, lower, bbox.corners[0]);
    Store(i, upper, bbox.corners[1]);
}
