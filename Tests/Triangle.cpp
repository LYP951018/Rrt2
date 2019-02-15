#include <catch.hpp>
#include <Rrt2/Geometries/Triangle.hpp>
#include "TestHelpers.hpp"

TEST_CASE("Triangle construction", "[Triangle]")
{
    const Vec3 p0 = Vec3{0.0f, 0.0f, 0.0f};
    const Vec3 p1 = Vec3{0.0f, 1.0f, 0.0f};
    const Vec3 p2 = Vec3{1.0f, 0.0f, 0.0f};
    Triangle triangle{p0, p1, p2};
    const BoundingBox& boundingBox = triangle.GetBoundingBox();
	REQUIRE(boundingBox.GetMin() == p0);
	REQUIRE(boundingBox.GetMax() == Vec3{ 1.0f, 1.0f, 0.0f });
}