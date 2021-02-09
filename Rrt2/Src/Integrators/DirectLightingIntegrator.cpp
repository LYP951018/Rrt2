#include "Rrt2/Integrators/DirectLightingIntegrator.hpp"
#include "Rrt2/Camera.hpp"
#include "Rrt2/Ray.hpp"
#include <algorithm>
#include "Rrt2/Geometries/GeometryBase.hpp"
#include "Rrt2/Scene.hpp"
#include "Rrt2/MaterialBase.hpp"
#include "Rrt2/Reflection.hpp"

namespace rrt
{
    struct LuminousEvaluator
    {
        RandomSampler& randomSampler;
        const Scene& scene;
        const Ray& ray;
        std::int32_t depth = 0;

        Spectrum EvalSingleLight(const LightBase& light,
            const SurfaceInteraction& interaction) {
            // f(x) = bdrf * cos\theta
            // g(x) = L(p, d)
            // left + right
            Spectrum left, right;
            float leftWeight = 0.0f, rightWeight = 0.0f;
            const Vec2f sampledPoint = randomSampler.Rand2D();

            const BsdfSampledEvalResult bxdfResult = interaction.bsdf->SampledEval(ray.speed,
                BxdfKind::kAll, sampledPoint);
            const float lightPdf = light.GetPdfForDir(interaction, bxdfResult.wiWorld);
            if (lightPdf == 0.0f) {
                // 以 BRDF 的重要性采样，灯 pdf = 0
                left = {};
            }
            else {
                // 将返回的世界坐标系的 wi 变换至 local
                const float cosAngle =
                    glm::dot(glm::vec3{interaction.shadingNormal}, bxdfResult.wiWorld);
                left = bxdfResult.f * cosAngle;
            }
            

            const SampleLiResult sampleLiResult = light.SampleLi(interaction);
            const float brdfPdf = interaction.bsdf->
        }

        Spectrum Eval()
        {
            std::optional<SurfaceInteraction> interactionOpt = scene.Trace(ray);
            if (!interactionOpt.has_value()) {
                return {};
            }
            const SurfaceInteraction& hitPoint = interactionOpt.value();
            Spectrum result;
            for (const std::unique_ptr<LightBase>& light : scene.GetLights())
            {
                const SampleLiResult sampledResult = light->SampleLi(hitPoint);
                if (!sampledResult.visibilityTester.Unoccluded(scene)) {
                    continue;
                }
                result += (sampledResult.illuminance / sampledResult.pdf);
            }

            // direct lighting 会重复计算光照？

            if (;
                interactionOpt.has_value())
            {
                SurfaceInteraction& interaction = interactionOpt.value();
                const GeometryBase& hitGeometry =
                    *scene.GetGeometryAt(interaction.geomId);
                const MaterialBase& material = *hitGeometry.material;
                material.ComputeScatteringFunctions(interaction);
                const Vec2f sampledPoint = randomSampler.Rand2D();
                const BsdfSampledEvalResult bsdfResult = interaction.bsdf->SampledEval(
                    ray.speed, BxdfKind::kAll, sampledPoint);
                const Ray incoming = interaction.SpawnRay(bsdfResult.wiWorld);
            }
        }
    };

    void DirectLightingIntegrator::Render(const Camera& camera, const Scene& scene)
    {
        const FilmSize& filmSize = camera.film->filmSize;
        for (std::uint32_t h = 0; h < filmSize.height; ++h)
        {
            for (std::uint32_t w = 0; w < filmSize.width; ++w)
            {
                const Vec2i pixelPos = Vec2i{w, h};
                m_pixelSampler.StartWithPixel(pixelPos);
                const Vec2f sampledPixelPos = m_pixelSampler.GetCameraSample();
                const Ray ray = camera.GetRay(sampledPixelPos);
                Film& film = *camera.film;
                const Spectrum incomingLuminous = Li(ray, scene);
                film.AddSample(pixelPos, incomingLuminous);
            }
        }
    }

    Spectrum DirectLightingIntegrator::Li(const Ray& ray, const Scene& scene)
    {
        return LuminousEvaluator{
            .randomSampler = m_randomSampler, .scene = scene, .ray = ray}
            .Eval();
    }

} // namespace rrt
