#include <catch2/catch.hpp>
#include <Accelerations/Bvh.hpp>
#include <Rrt2/Geometries/TriangleMesh.hpp>
#include <Rrt2/Scene.hpp>

using namespace rrt;

TEST_CASE("Bvh construction", "[Bvh]")
{
    std::vector<Vec3f> vertexBuffer1{Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{0.0f, 1.0f, 0.0f},
                                     Vec3f{1.0f, 1.0f, 0.0f}, Vec3f{1.0f, 1.0f, 0.0f},
                                     Vec3f{1.0f, 0.0f, 0.0f}, Vec3f{0.0f, 0.0f, 0.0f}};
    std::vector<TriangleIndices> indices1{TriangleIndices{0, 1, 2}, TriangleIndices{3, 4, 5}};
    const std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    scene->AddTriangleMesh(
        std::make_unique<TriangleMesh>(std::move(vertexBuffer1), std::move(indices1)));

    std::vector<Vec3f> vertexBuffer2{Vec3f{1.0f, 0.0f, 0.0f}, Vec3f{1.0f, 1.0f, 0.0f},
                                     Vec3f{2.0f, 1.0f, 0.0f}, Vec3f{2.0f, 1.0f, 0.0f},
                                     Vec3f{2.0f, 0.0f, 0.0f}, Vec3f{1.0f, 0.0f, 0.0f}};
    std::vector<TriangleIndices> indices2{TriangleIndices{0, 1, 2}, TriangleIndices{3, 4, 5}};
    scene->AddTriangleMesh(
        std::make_unique<TriangleMesh>(std::move(vertexBuffer2), std::move(indices2)));

    scene->Ready();
}