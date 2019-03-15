#include <catch.hpp>
#include "TestHelpers.hpp"
#include <Rrt2/Accelerations/SimdBoundingBox.hpp>
#include <Rrt2/Accelerations/PackedRay.hpp>
#include <Rrt2/BoundingBox.hpp>
#include <Rrt2/Ray.hpp>

TEST_CASE("SimdBoundingBox intersection", "[SimdBoundingBox]")
{
    BoundingBox boxes[4] = {
        BoundingBox{
            {MakeFloats(1.0f, 1.0f, 1.0f, 1.0f), MakeFloats(2.0f, 3.0f, 4.0f, 1.0f)},
        },
        BoundingBox{
            {MakeFloats(2.0f, 3.0f, 4.0f, 1.0f), MakeFloats(6.0f, 9.0f, 12.0f, 1.0f)},
        },
        BoundingBox{
            {MakeFloats(12.0f, 18.0f, 10.0f, 1.0f), MakeFloats(22.0f, 28.0f, 20.0f, 1.0f)},
        },
        BoundingBox{
            {MakeFloats(-2.0f, -2.0f, -2.0f, 1.0f), MakeFloats(0.0f, 0.0f, 0.0f, 1.0f)},
        }};

    SimdBoundingBox simd;
    int i = 0;
    for (const BoundingBox& box : boxes)
    {
        simd.Set(i, box);
        ++i;
    }
    Ray ray;
    ray.origin = MakeFloats(-1.0f, -1.0f, 1.0f, 0.0f);
    ray.speed = MakeFloats(0.0f, 0.0f, -1.0f, 0.0f);
    PackedRay packed{ray};
    const int mask = simd.Hit(packed, 0.0f, 10.0f);
    CHECK(mask == 8);
}