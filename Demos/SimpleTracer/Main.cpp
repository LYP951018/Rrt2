#include <Rrt2/Scene.hpp>
#include <Rrt2/Camera.hpp>
#include <Rrt2/Geometries/TriangleMesh.hpp>
#include <pcg_variants.h>
#include <Rrt2/Stb/stb_image_write.h>
#include <Rrt2/SceneObjLoader.hpp>
#include <numbers>
#include <ctime>
#include <numbers>

int main()
{
    using namespace rrt;
    constexpr std::uint32_t kFilmWidth = 60;
    constexpr std::uint32_t kFilmHeight = 60;
    constexpr std::uint32_t kSamplesPerPixel = 4;
    const std::unique_ptr<Scene> mainScene = std::make_unique<Scene>();
    Camera mainCamera{PerspectiveParam{
                          .near = 1.0f,
                          .far = 1000.0f,
                          .fov = std::numbers::pi_v<float> / 4.0f,
                      },
                      glm::identity<glm::mat4>(),
                      FilmSize{.width = kFilmWidth, .height = kFilmHeight}};
    pcg32_random_t randState;
    pcg32_srandom_r(&randState, std::time(0), (intptr_t)&randState);
    constexpr float sqrt3f = std::numbers::sqrt3_v<float>;

    constexpr float kTriangleZ = 10.0f;
    // 美丽的等边三角形
    /*std::vector<Vec3f> positions = {
        Vec3f{0.0f, sqrt3f / 2.0f - 0.5f, kTriangleZ},
        Vec3f{0.5f, -0.5f, kTriangleZ}, Vec3f{-0.5f, -0.5f, kTriangleZ}};

    std::vector<TriangleIndices> indices = {{0, 1, 2}};*/
    bool added = AddMeshesFromObj(*mainScene, "./models/cornell_box.obj");
  /*  mainScene->AddTriangleMesh(
        std::make_unique<TriangleMesh>(positions, indices));*/
    assert(added);
    mainScene->Ready();
    std::vector<Ray> rays;
    mainCamera.GenerateRays(randState, rays, kSamplesPerPixel);
    std::vector<unsigned char> pixels(kFilmHeight * kFilmWidth * 4);
    for (int i = 0; i < kFilmHeight; ++i)
    {
        for (int j = 0; j < kFilmWidth; ++j)
        {
            Vec3f color = {};
            for (int k = 0; k < kSamplesPerPixel; ++k)
            {
                const Ray& ray = rays[i * kFilmWidth * kSamplesPerPixel +
                                      j * kSamplesPerPixel + k];
                const std::optional<HitRecord> hitRecord =
                    mainScene->Trace(ray, 0.0f, 10.0f);
                if (hitRecord)
                {
                    color += Vec3f{1.0f, 1.0f, 1.0f};
                    const GeometryBase* hitGeometry =
                        mainScene->GetGeometryAt(hitRecord.value().geomId);
                }
            }
            color /= kSamplesPerPixel;
            const std::size_t pixelStart = i * kFilmWidth * 4 + j * 4;
            pixels[pixelStart] = 255 * color.x;
            pixels[pixelStart + 1] = 255 * color.y;
            pixels[pixelStart + 2] = 255 * color.z;
            pixels[pixelStart + 3] = 255;
        }
    }
    const int writtenResult =
        stbi_write_png(".\\result.png", kFilmWidth, kFilmHeight, 4,
                       pixels.data(), kFilmWidth * kSamplesPerPixel);
    assert(writtenResult != 0);
}