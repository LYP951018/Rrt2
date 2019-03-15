#include <catch.hpp>
#include <Rrt2/Camera.hpp>
#include <Rrt2/Ray.hpp>
#include <pcg_variants.h>
#include <ctime>

TEST_CASE("Generate rays", "Camera")
{
    pcg32_random_t randState;
    pcg32_srandom_r(&randState, std::time(0), (intptr_t)&randState);
    Camera mainCamera{50, 50, 0.5f, 1.0f};
    mainCamera.SetLookAt({}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});
    AlignedVec<Ray> rays;
    mainCamera.GenerateRays(randState, rays, 4);
    CHECK(rays.size() == 50 * 50 * 4);
    const float sizePerPixel = 1.0f / 50.0f;
    for (unsigned i = 0; i < 50; ++i)
    {
        for (unsigned j = 0; j < 50; ++j)
        {
            float lower = j * 50 + i - 0.5f;
            float upper = lower + 1.0f;
            for (unsigned k = 0; k < 4; ++k)
            {
                const Ray ray = rays[i * 50 + j * 4 + k];
                const PackedFloats origin = ray.origin;
                auto [x, y, z, w] = origin.m128_f32;
                CHECK(x >= -0.5f);
                CHECK(x <= 0.5f);
                CHECK(z == 0.0f);
            }
        }
    }
}