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


TEST_CASE("cornell box", "[bvh]")
{
    const std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    std::vector<Vec3f> vertices = {
        Vec3f{0.0f, 0.0f, 559.2f},   Vec3f{0.0f, 0.0f, 0.0f},
        Vec3f{0.0f, 548.0f, 0.0f},   Vec3f{0.0f, 548.8f, 559.2f},
        Vec3f{549.6f, 0.0f, 559.2f}, Vec3f{0.0f, 0.0f, 559.2f},
        Vec3f{0.0f, 548.8f, 559.2f}, Vec3f{556.0f, 548.8f, 559.2f}};

    std::vector<TriangleIndices> indices = {
        TriangleIndices {0, 1, 2},
        TriangleIndices{1, 2, 3},
        TriangleIndices {4, 5, 6},
        TriangleIndices{5, 6, 7},
    };
    auto mesh = std::make_unique<TriangleMesh>(std::move(vertices),
        std::move(indices));
    scene->AddTriangleMesh(std::move(mesh));
    scene->Ready();
    Ray ray;
    ray.origin = glm::vec3(0.0f, 0.0f, -100.0f);
    ray.speed = glm::vec3(0.2f, 0.2f, 0.9798f);
    auto record = scene->Trace(ray, 0.0f, 1000.0f);
}