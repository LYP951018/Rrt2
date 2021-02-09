#include <Rrt2/Materials/MattMaterial.hpp>
#include <Rrt2/Reflection.hpp>

namespace rrt
{
    DiffuseOnlyMaterial::DiffuseOnlyMaterial(
        std::shared_ptr<const TextureBase<Spectrum>> kd)
        : m_kd(std::move(kd))
    {}

    void DiffuseOnlyMaterial::ComputeScatteringFunctions(SurfaceInteraction& interaction)
    {
        std::unique_ptr<Bsdf> bsdf = std::make_unique<Bsdf>(interaction);
        const Spectrum diffuseFactor = m_kd->Eval(interaction);
        bsdf->bxdfs.push_back(std::make_unique<LambertianReflection>(diffuseFactor));
        interaction.bsdf = std::move(bsdf);
    }
} // namespace rrt
