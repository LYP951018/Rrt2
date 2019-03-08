#include <catch.hpp>
#include "TestHelpers.hpp"
#include <Rrt2/Accelerations/SimdTriangle.hpp>
#include <Rrt2/Geometries/TriangleMesh.hpp>
#include <Rrt2/Ray.hpp>

TEST_CASE("Triangle intersection", "[SimdTriangle]")
{
    SingleTriangle triangles[4] = {SingleTriangle{Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{1.0f, 0.0f, 0.0f},
                                                  Vec3f{0.5f, 0.866025f, 0.612372f}},
                                   SingleTriangle{Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{-1.0f, 0.0f, 0.0f},
                                                  Vec3f{-0.5f, 0.866025f, 0.612372f}},
                                   SingleTriangle{Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{1.0f, 0.0f, 0.0f},
                                                  Vec3f{0.5f, -0.866025f, 0.612372f}},
                                   SingleTriangle{Vec3f{0.0f, 0.0f, 0.0f}, Vec3f{-1.0f, 0.0f, 0.0f},
                                                  Vec3f{-0.5f, -0.866025f, 0.612372f}}};
    SimdTriangle simd;
    for (std::uint32_t i = 0; i < 4; ++i)
    {
        simd.Set(i, triangles[i], i, 0);
    }
    Ray ray;
    ray.origin = MakeFloats(-0.5f, 0.5f, 0.5f, 0.0f);
    ray.speed = MakeFloats(0.0f, 0.0f, -1.0f, 0.0f);
    ray.Finalize();
    const std::optional<HitRecord> record = simd.Hit(ray, 0.0f, 1.0f);
    CHECK(record.has_value());
    const HitRecord& rec = record.value();
    CHECK(rec.primId == 1);
    CHECK(rec.geomId == 0);
    // TODO: check position
}