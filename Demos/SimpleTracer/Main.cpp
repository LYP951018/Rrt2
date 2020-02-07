#include <Rrt2/Scene.hpp>
#include <Rrt2/Camera.hpp>
#include <pcg_variants.h>
#include <numbers>
#include <ctime>


int main()
{
    const std::unique_ptr<Scene> mainScene = std::make_unique<Scene>();
    Camera mainCamera{
        PerspectiveParam {
            .near = 1.0f,
            .far = 1000.0f,
            .fov =
        }
    };
    pcg32_random_t randState;
    pcg32_srandom_r(&randState, std::time(0), (intptr_t)&randState);
    AlignedVec<Ray> rays;
    mainCamera.GenerateRays(randState, rays, 4);
    std::vector<Vec3f> positions = {
        Vec3f{}
    }
    mainScene->AddGeometry(std::make_unique<TriangleMesh>()
    for (int i = 0; i < 50; ++i)
    {
        for (int j = 0; j < 50; ++j)
        {
            Vec3f color = {};
            for (int k = 0; k < 4; ++k)
            {
                const Ray& ray = rays[i * j * 4 + j * 4 + k];
                const std::optional<HitRecord> hitRecord =
                    mainScene->Trace(ray, 0.0f, 10.0f);
                if (hitRecord)
                {
                    color = { 1.0f, 1.0f, 1.0f };
                    const GeometryBase* hitGeometry =
                        mainScene->GetGeometryAt(hitRecord.value().geomId);
                }
            }
        }
    }
}