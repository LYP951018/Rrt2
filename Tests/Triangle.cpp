#include <catch2/catch.hpp>
#include "TestHelpers.hpp"
#include <Accelerations/PackedTriangle.hpp>
#include <Accelerations/PackedRay.hpp>
#include <Rrt2/Geometries/TriangleMesh.hpp>
#include <Rrt2/Ray.hpp>

using namespace rrt;

TEST_CASE("Triangle intersection", "[PackedTriangle]")
{
    SingleTriangle triangles[4] = {SingleTriangle{Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{1.0f, 0.0f, 0.0f},
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
    const std::optional<HitRecord> record = simd.Load().Hit(packed, ray, 0.0f, 1.0f);
    CHECK(record.has_value());
    const HitRecord& rec = record.value();
    CHECK(rec.primId == 1);
    CHECK(rec.geomId == 0);
    // TODO: check position
}