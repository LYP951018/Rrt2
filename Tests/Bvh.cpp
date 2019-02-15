#include <catch.hpp>
#include <Rrt2/Accelerations/Bvh.hpp>
#include <Rrt2/Geometries/Triangle.hpp>

TEST_CASE("OctTree construction", "[OctTree]")
{
    OctTree tree{BoundingBox{Vec3{-1.0f, -1.0f, -1.0f}, Vec3{1.0f, 1.0f, 1.0f}}};
	const std::unique_ptr<Triangle> triangle1 = std::make_unique<Triangle>(
		Vec3{ 0.0f, 0.0f, 0.0f }, Vec3{ 0.0f, 1.0f, 0.0f }, Vec3{ 1.0f, 0.0f, 0.0f });
	tree.Insert(triangle1.get());
	const std::unique_ptr<Triangle> triangle2 = std::make_unique<Triangle>(
		Vec3{ 0.0f, 0.0f, 0.0f }, Vec3{ 0.0f, 1.0f, 0.0f }, Vec3{ -1.0f, 0.0f, 0.0f });
	tree.Insert(triangle2.get());
	const OctTreeNode* const root = tree.GetRoot();
	REQUIRE(root->children[2] != nullptr);
	REQUIRE(root->children[3] != nullptr);
}