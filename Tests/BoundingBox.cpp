#include <catch2/catch.hpp>
#include "TestHelpers.hpp"
#include <Rrt2/BoundingBox.hpp>
#include <Rrt2/Accelerations/PackedRay.hpp>
#include <Rrt2/Accelerations/PackedBoundingBox.hpp>
#include <Rrt2/BoundingBox.hpp>
#include <Rrt2/Ray.hpp>

using namespace rrt;

TEST_CASE("PackedBoundingBox intersection", "[PackedBoundingBox]")
{
    BoundingBoxStorage boxes[4] = {
        BoundingBoxStorage {
            .corners = {glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, glm::vec4{2.0f, 3.0f, 4.0f, 1.0f}},
        },
        BoundingBoxStorage{
            .corners = {glm::vec4{2.0f, 3.0f, 4.0f, 1.0f}, glm::vec4{6.0f, 9.0f, 12.0f, 1.0f}},
        },
        BoundingBoxStorage{
            .corners = {glm::vec4{12.0f, 18.0f, 10.0f, 1.0f}, glm::vec4{22.0f, 28.0f, 20.0f, 1.0f}},
        },
        BoundingBoxStorage{
            .corners = {glm::vec4{-2.0f, -2.0f, -2.0f, 1.0f}, glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}},
        }};

    PackedBoundingBoxStorage simd;
    int i = 0;
    for (const BoundingBoxStorage& box : boxes)
    {
        simd.Set(i, box);
        ++i;
    }
    Ray ray;
    ray.origin = glm::vec4(-1.0f, -1.0f, 1.0f, 0.0f);
    ray.speed = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    PackedRay packed{ray};
    const int mask = simd.Load().Hit(packed, 0.0f, 10.0f);
    CHECK(mask == 8);
}

TEST_CASE("PackedBoundingBox math functions", "[PackedBoundingBox]")
{
    const BoundingBoxStorage testBBox{
            .corners = {glm::vec4{1.0f, 2.0f, 3.0f, 1.0f}, glm::vec4{4.0f, 7.0f, 9.0f, 1.0f}},
    };
    const BoundingBox bbox = testBBox.Load();
    const Float4 size = bbox.GetSize();

    CHECK(M128Comparer{ size } == M128Comparer{ MakeFloats(3.0f, 5.0f, 6.0f, 0.0f) });

    const float halfArea = bbox.GetHalfArea();

    CHECK(Approx{ halfArea } == 63.0f);
}
