#include <catch2/catch.hpp>
#include "TestHelpers.hpp"
#include <Rrt2/Accelerations/PackedTriangle.hpp>
#include <Rrt2/Accelerations/PackedRay.hpp>
#include <Rrt2/Geometries/TriangleMesh.hpp>
#include <Rrt2/Ray.hpp>
#include <Rrt2/PrimRef.hpp>

using namespace rrt;

TEST_CASE("Triangle intersection", "[PackedTriangle]")
{
    SingleTriangle triangles[4] = {
        SingleTriangle{Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{1.0f, 0.0f, 0.0f},
                       Vec3f{0.5f, 0.866025f, 0.612372f}},
        SingleTriangle{Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{-1.0f, 0.0f, 0.0f},
                       Vec3f{-0.5f, 0.866025f, 0.612372f}},
        SingleTriangle{Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{1.0f, 0.0f, 0.0f},
                       Vec3f{0.5f, -0.866025f, 0.612372f}},
        SingleTriangle{Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{-1.0f, 0.0f, 0.0f},
                       Vec3f{-0.5f, -0.866025f, 0.612372f}}};
    PackedTriangleStorage simd;
    for (std::uint32_t i = 0; i < 4; ++i)
    {
        simd.Set(i, triangles[i], i, 0);
    }
    Ray ray;
    ray.origin = glm::vec3(-0.5f, 0.5f, 0.5f);
    ray.speed = glm::vec3(0.0f, 0.0f, -1.0f);
    PackedRay packed{ray};
    const std::optional<HitRecord> record =
        simd.Load().Hit(packed, ray, 0.0f, 1.0f);
    CHECK(record.has_value());
    const HitRecord& rec = record.value();
    CHECK(rec.primId == 1);
    CHECK(rec.geomId == 0);
    // TODO: check position
}

TEST_CASE("Triangle mesh FillPrimitiveArray") {
    std::vector<Vec3f> vertices = {
        Vec3f{552.8f, 0.0f, 0.0f}, Vec3f{0.0f, 0.0f, 0.0f},
        Vec3f{0.0f, 0.0f, 559.2f}, Vec3f{549.6f, 0.0f, 559.2f}};
    std::vector<TriangleIndices> indices = {
        TriangleIndices{.v0 = 0, .v1 = 1, .v2 = 2},
        TriangleIndices{.v0 = 1, .v1 = 2, .v2 = 3}};
    TriangleMesh mesh{std::move(vertices), std::move(indices)};
    mesh.SetGeomId(1);
    AlignedVec<PrimRefStorage> primRefStorages;
    mesh.FillPrimitiveArray(primRefStorages);
    CHECK(primRefStorages.size() == 2);
    PrimRefStorage storage0 = primRefStorages[0];
    CHECK(storage0.lower == glm::vec3{0.0f, 0.0f, 0.0f});
    CHECK(storage0.geomId == 1);
    CHECK(storage0.primId == 0);
    CHECK(storage0.upper == glm::vec3(552.8f, 0.0f, 559.2f));
    PrimRefStorage storage1 = primRefStorages[1];
    CHECK(storage1.lower == glm::vec3{0.0f, 0.0f, 0.0f});
    CHECK(storage1.geomId == 1);
    CHECK(storage1.primId == 1);
    CHECK(storage1.upper == glm::vec3(549.6f, 0.0f, 559.2f));
}

