#include <Rrt2/Reflection.hpp>
#include <Rrt2/MathBasics.hpp>
#include <cassert>
#include <bit>
#include <algorithm>

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

    Spectrum Bxdf::SampledEval(const Vec3f& wo, Vec3f& wi, const glm::vec2& sample,
                               float& pdf) const
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
                spectrum += sampledSpectrum * AbsCosTheta(wi) / pdf;
        }
        return spectrum / (float)samples.size();
    }

    float Bxdf::GetPdf(const Vec3f& wo, const Vec3f& wi) const
    {
        return IsSameHemisphere(wo, wi) ? AbsCosTheta(wi) * kInvPi : 0.0f;
    }

    bool Bxdf::IsMatch(BxdfKind destKind) const { return (kind & destKind) == destKind; }

    Spectrum SpecularReflection::Eval(const Vec3f& wo, const Vec3f& wi) const
    {
        return Spectrum{0.0f};
    }

    Spectrum SpecularReflection::SampledEval(const Vec3f& wo, Vec3f& wi, const glm::vec2&,
                                             float& pdf) const
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
                                               const glm::vec2&, float& pdf) const
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

    Spectrum Bsdf::Eval(const Vec3f& woWorld, const Vec3f& wiWorld, BxdfKind kind) const
    {
        const Vec3f localWO = WorldToLocal(woWorld);
        const Vec3f localWI = WorldToLocal(woWorld);
        const bool isReflect = IsReflect(localWO, localWI);
        Spectrum f{};
        for (const std::unique_ptr<Bxdf>& bxdf : bxdfs)
        {
            const bool matchedReflect = isReflect && bxdf->IsReflection();
            const bool matchedTransmission = !isReflect && bxdf->IsTransmission();
            const bool isMatched =
                (matchedReflect || matchedTransmission) && bxdf->IsMatch(kind);
            if (isMatched)
            {
                f += bxdf->Eval(localWO, localWI);
            }
        }
        return f;
    }

    BsdfSampledEvalResult Bsdf::SampledEval(const Vec3f& woWorld, BxdfKind kind, const Vec2f& u)
    {
        BsdfSampledEvalResult evalResult{};
        const std::vector<std::uint32_t> matchedIndices = GetMatchedBxdfIndices(kind);
        const std::uint32_t matchedCount =
            static_cast<std::uint32_t>(matchedIndices.size());
        if (matchedCount == 0)
        {
            evalResult.f = Spectrum{};
            return evalResult;
        }
        const float mappedToIndex = u.x * matchedCount;
        const std::uint32_t selectedIndex =
            std::min(static_cast<std::uint32_t>(mappedToIndex), matchedCount - 1);
        const Bxdf& bxdf = *bxdfs[matchedIndices[selectedIndex]];
        const Vec2f remapped = {mappedToIndex - selectedIndex, u.y};
        const Vec3f localWO = WorldToLocal(woWorld);
        Vec3f localWI;
        evalResult.f = bxdf.SampledEval(localWO, localWI, remapped, evalResult.pdf);
        evalResult.wiWorld = LocalToWorld(localWI);
        if (matchedCount == 1)
        {
            return evalResult;
        }

        // 如果是镜面反射此时拿到的 pdf 是 1，不要再加了
        if (!bxdf.IsSpecular())
        {
            for (std::uint32_t index : matchedIndices)
            {
                const Bxdf& matchedBxdf = *bxdfs[index];
                evalResult.pdf += matchedBxdf.GetPdf(localWO, localWI);
            }
        }

        const bool isReflect = IsSameHemisphere(localWI, localWO);
        for (std::uint32_t index : matchedIndices)
        {
            const Bxdf& matchedBxdf = *bxdfs[index];
            const bool matchedReflect = isReflect && matchedBxdf.IsReflection();
            const bool matchedTransmission = !isReflect && matchedBxdf.IsTransmission();
            if (matchedReflect || matchedTransmission)
            {
                evalResult.f += matchedBxdf.Eval(localWO, localWI);
            }
        }
        return evalResult;
    }

    std::vector<std::uint32_t> Bsdf::GetMatchedBxdfIndices(BxdfKind kind)
    {
        std::vector<std::uint32_t> indices;
        for (std::size_t i = 0; i < bxdfs.size(); ++i)
        {
            const Bxdf& bxdf = *bxdfs[i];
            if (bxdf.IsMatch(kind))
            {
                indices.push_back(static_cast<std::uint32_t>(i));
            }
        }
        return indices;
    }

    bool Bsdf::IsReflect(const Vec3f& wo, const Vec3f& wi) const
    {
        return glm::dot(wo, m_geometricNormal) * glm::dot(wi, m_geometricNormal) > 0;
    }

} // namespace rrt