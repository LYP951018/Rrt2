#include <Rrt2/Reflection.hpp>
#include <Rrt2/MathBasics.hpp>
#include <cassert>

namespace rrt
{
    glm::vec2 UniformSampleDisk(const glm::vec2& sample)
    {
        const float r = std::sqrt(sample.x);
        const float theta = 2 * kPi * sample.y;
        return glm::vec2{r * std::cos(theta), r * std::sin(theta)};
    }

    glm::vec2 ConcentricSampleDisk(const glm::vec2& sample)
    {
        assert(false);
        return {};
    }

    Vec3f CosineSampleHemisphere(const glm::vec2& sample)
    {
        glm::vec2 xz = UniformSampleDisk(sample);
        const float y = std::sqrt(1 - xz.x * xz.x);
        return Vec3f{xz.x, y, xz.y};
    }

    Fresnel::~Fresnel() = default;

    Spectrum FresnelNoOp::Eval(float) const { return Spectrum{1.0f}; }

    Bxdf::~Bxdf() = default;

    Spectrum Bxdf::SampledEval(const Vec3f& wo, Vec3f& wi,
                               const glm::vec2& sample, float& pdf) const
    {
        wi = CosineSampleHemisphere(sample);
        pdf = GetPdf(wo, wi);
        return Eval(wo, wi);
    }

    Spectrum Bxdf::Rho(const Vec3f& wo, std::span<const glm::vec2> samples)
    {
        Spectrum spectrum{0.0f};
        for (const glm::vec2& sample : samples)
        {
            Vec3f wi;
            float pdf = 0.0f;
            Spectrum sampledSpectrum = SampledEval(wo, wi, sample, pdf);
            if (pdf > 0.0f)
                spectrum += sampledSpectrum;
        }
        return spectrum / (float)samples.size();
    }

    float Bxdf::GetPdf(const Vec3f& wo, const Vec3f& wi) const
    {
        return IsSameHemisphere(wo, wi) ? AbsCosTheta(wi) * kInvPi : 0.0f;
    }

    Spectrum SpecularReflection::Eval(const Vec3f& wo, const Vec3f& wi) const
    {
        return Spectrum{0.0f};
    }

    Spectrum SpecularReflection::SampledEval(const Vec3f& wo, Vec3f& wi,
                                             const glm::vec2&, float& pdf) const
    {
        // 我们现在是在 BRDF coord, normal 就是 (0, 1, 0)
        wi = Vec3f{-wo.x, wo.y, -wo.z};
        pdf = 1.0f;
        return m_fresnel->Eval(CosTheta(wi)) * m_r / AbsCosTheta(wi);
    }

    Spectrum LambertianReflection::Eval(const Vec3f& wo, const Vec3f& wi) const
    {
        return m_r / kPi;
    }

    Spectrum LambertianReflection::SampledEval(const Vec3f& wo, Vec3f& wi,
                                               const glm::vec2&,
                                               float& pdf) const
    {
        // 我们现在是在 BRDF coord, normal 就是 (0, 1, 0)
        wi = Vec3f{-wo.x, wo.y, -wo.z};
        pdf = 1.0f / kPi;
        return m_r / kPi;
    }

    Spectrum LambertianReflection::Rho(const Vec3f& wo,
                                       std::span<const glm::vec2> samples)
    {
        return m_r;
    }

} // namespace rrt