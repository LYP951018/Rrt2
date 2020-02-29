#include <Rrt2/Simd.hpp>
#include <Rrt2/Vec3.hpp>
#include "TestHelpers.hpp"
#include <catch2/catch.hpp>

using namespace rrt;

TEST_CASE("Cross3Packed", "Simd")
{
    Vec3fPacked packed1{MakeFloats(1.0f, 2.0f, 3.0f, 0.0f), MakeFloats(2.0f, 3.0f, 4.0f, 0.0f),
                        MakeFloats(3.0f, 4.0f, 5.0f, 0.0f)};
    Vec3fPacked packed2{MakeFloats(3.0f, 2.0f, 1.0f, 0.0f), MakeFloats(4.0f, 3.0f, 2.0f, 0.0f),
                        MakeFloats(5.0f, 4.0f, 3.0f, 0.0f)};
    //(1, 2, 3) x (3, 4, 5)
    //(2, 3, 4) x (2, 3, 4)
    //(3, 4, 5) x (1, 2, 3)
    Vec3fPacked result = Cross3(packed1, packed2);
    CHECK_M128_EQ(result.x, MakeFloats(-2.0f, 0.0f, 2.0f, 0.0f));
    CHECK_M128_EQ(result.y, MakeFloats(4.0f, 0.0f, -4.0f, 0.0f));
    CHECK_M128_EQ(result.z, MakeFloats(-2.0f, 0.0f, 2.0f, 0.0f));
}

TEST_CASE("Dot3", "Simd") {}

TEST_CASE("MinElement", "Simd")
{
    PackedFloats min = SelectMinElement(MakeFloats(4.0f, 2.0f, 3.0f, 3.0f));
    CHECK_M128_EQ(min, MakeFloats(2.0f, 2.0f, 2.0f, 2.0f));
}
