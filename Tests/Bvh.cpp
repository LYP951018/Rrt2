#include <catch2/catch.hpp>
#include <Rrt2/Accelerations/Bvh.hpp>
#include <Rrt2/Geometries/TriangleMesh.hpp>
#include <Rrt2/Scene.hpp>
#include <Rrt2/SceneObjLoader.hpp>

using namespace rrt;

TEST_CASE("Bvh construction with one spliting", "[Bvh]")
{
    const std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    AddMeshesFromObj(*scene, "./models/BvhTest1.obj");
    scene->Ready();
}


TEST_CASE("Bvh construction with multi levels", "[Bvh]")
{
    const std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    AddMeshesFromObj(*scene, "./models/BvhMultiLevel.obj");
    scene->Ready();
}