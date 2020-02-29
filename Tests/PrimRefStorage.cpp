#include <Rrt2/PrimRef.hpp>
#include <Rrt2/Simd.hpp>
#include <catch2/catch.hpp>
#include <Rrt2/MathBasics.hpp>
#include "TestHelpers.hpp"

using namespace rrt;

TEST_CASE("PrimRefStorage construction", "[PrimRef]") 
{
    BoundingBox boundingBox{.corners = {MakeFloats(1.0f, 2.0f, 3.0f, 0.0f),
                                        MakeFloats(4.0f, 5.0f, 6.0f, 0.0f)}};
    PrimRefStorage primRef{boundingBox, 10, 11};
    CHECK(primRef.geomId == 10);
    CHECK(primRef.primId == 11);
    CHECK(primRef.lower == glm::vec3(1.0f, 2.0f, 3.0f));
    CHECK(primRef.upper == glm::vec3(4.0f, 5.0f, 6.0f));

    const PrimRef loaded = primRef.Load();
    Float4 expectedLower = MakeFloats(1.0f, 2.0f, 3.0f, bit_cast<float>(10));
    Float4 expectedUpper = MakeFloats(4.0f, 5.0f, 6.0f, bit_cast<float>(11));

    CHECK_M128_EQ(loaded.upper, expectedUpper);
    CHECK_M128_EQ(loaded.lower, expectedLower);
}