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


TEST_CASE("Bvh with multi levels", "[Bvh]")
{
    const std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    AddMeshesFromObj(*scene, "./models/BvhMultiLevel.obj");
    scene->Ready();

    Ray ray;
    ray.origin = glm::vec3(-3.087f, 1.2416f, 0.0f);
    ray.speed = glm::vec3(0.0f, 0.0f, 1.0f);
    const std::optional<SurfaceInteraction> record =
        scene->Trace(ray, 0.0f, 10.0f);
    CHECK(record.has_value());
    const SurfaceInteraction& rec = record.value();
   // CHECK(rec.primId == 1);
    CHECK(rec.geomId == 3);
}