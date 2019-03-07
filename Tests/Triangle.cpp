#include <catch.hpp>
#include "TestHelpers.hpp"

TEST_CASE("TriangleMesh construction", "[TriangleMesh]")
{
    const Vec3f p0 = Vec3f{0.0f, 0.0f, 0.0f};
    const Vec3f p1 = Vec3f{0.0f, 1.0f, 0.0f};
    const Vec3f p2 = Vec3f{1.0f, 0.0f, 0.0f};
    /* TriangleMesh triangle{p0, p1, p2};
     const BoundingBox& boundingBox = triangle.GetBoundingBox();
         REQUIRE(boundingBox.GetMin() == p0);
         REQUIRE(boundingBox.GetMax() == Vec3f{ 1.0f, 1.0f, 0.0f });*/
}