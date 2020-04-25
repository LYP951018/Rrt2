#pragma once

#include <cstdint>
#include <span>
#include <memory>
#include <glm/geometric.hpp>
#include "EnumFlags.hpp"
#include "Spectrum.hpp"
#include "Vec3.hpp"

namespace rrt
{
    enum class BxdfKind : std::uint32_t
    {
        kReflection = 1,
        kTransmission = 2,
        kDiffuse = 1 << 2,
        kGlossy = 1 << 3,
        kSpecular = 1 << 4,
        kAll = kReflection | kTransmission | kDiffuse | kGlossy | kSpecular
    };

    RRT_DEF_FLAG_ENUM_OPS(BxdfKind)

    // 球坐标相关函数
    // 球坐标的 \theta、\phi
    // 要求传入的 v 是归一化后的
    // BRDF coord

    inline float CosTheta(const Vec3f& v) { return v.y; }
    inline float AbsCosTheta(const Vec3f& v) { return std::abs(v.y); }

    inline float Cos2Theta(const Vec3f& v) { return v.y * v.y; }

    inline Vec3f Reflect(const Vec3f& wo, const Vec3f& normal)
    {
        return -wo + 2.0f * glm::dot(wo, normal) * normal;
    }

    inline bool IsSameHemisphere(const Vec3f& wo, const Vec3f& wi)
    {
        return CosTheta(wo) * CosTheta(wi) > 0.0f;
    }

    /*
     * p(\theta, \phi) = 1 / \pi cos\theta sin\theta
     * 使用 Malley's method，在半球上均匀取点实际上和在圆上均匀取点一样
     * 即 (r, \phi) = (\sqrt{\epsilon_1}, 2\pi\epsilon_2)
     * 返回取样的点的坐标
     */
    glm::vec2 UniformSampleDisk(const glm::vec2& sample);

    /* UniformSampleDisk 对原来 (\epsilon_1, \epsilon_2) 的点扭曲的太严重
     * 导致偏离原来的位置太远
     * 这是个改进版本
     * \theta x = 2y \pi / 8
     * r = x
     * \theta = y / x * \pi / 4
     * */
    glm::vec2 ConcentricSampleDisk(const glm::vec2& sample);

    /*
     * 以 (0, 1, 0) 为法线，按 cos\theta 的 pdf 生成向量
     */
    Vec3f CosineSampleHemisphere(const glm::vec2& sample);

    // 用于求解反射折射比例。
    class Fresnel
    {
      public:
        virtual ~Fresnel();
        // 返回被反射的比例。
        // cosI：入射角
        virtual Spectrum Eval(float cosI) const = 0;
    };

    class FresnelNoOp : public Fresnel
    {
      public:
        Spectrum Eval(float cosI) const override;
    };

    // BTDF, BRDF, ...
    class Bxdf
    {
      public:
        Bxdf(BxdfKind _kind) : kind{_kind} {}

        virtual ~Bxdf();
        // 对入射光线 wi 与出射光线 wo 求解 BXDF
        virtual Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const = 0;
        // 对某些满足 delta distribution 的表面（镜面反射）以及
        // 需要随机采样得到入射光线的的表面使用 SampledEval
        // sample 是输入的 (0, 1) 范围内的值，用于 importance sampling
        virtual Spectrum SampledEval(const Vec3f& wo, Vec3f& wi,
                                     const glm::vec2& sample, float& pdf) const;

        // 对出射方向为 \omega_o 的入射方向的 BRDF 积分得到的
        // hemispherical-directional reflectance
        virtual Spectrum Rho(const Vec3f& wo,
                             std::span<const glm::vec2> samples);

        virtual float GetPdf(const Vec3f& wo, const Vec3f& wi) const;

        const BxdfKind kind;
    };

    /* \iint brdf * L_i(\omega_i) |cos\theta| d\omega_i =
      F_r(\omegma_r)L_i(\omega_r) \omega_r 是 \omega_o 的反射光线 R(\omega_o,
      normal) brdf = F_r(\omegma_r) / |cos\theta|
      */
    class SpecularReflection : public Bxdf
    {
      public:
        SpecularReflection(std::unique_ptr<Fresnel> fresnel, const Spectrum& r)
            : Bxdf{BxdfKind::kSpecular | BxdfKind::kReflection},
              m_fresnel{std::move(fresnel)},
              m_r{r}
        {}

        Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const override;
        Spectrum SampledEval(const Vec3f& wo, Vec3f& wi,
                             const glm::vec2& sample,
                             float& pdf) const override;
        // FIXME: pbrt 里面对 specular reflection 的 rho 求解似乎有问题

      private:
        std::unique_ptr<const Fresnel> m_fresnel;
        // 贴图颜色之类的
        const Spectrum m_r;
    };

    class LambertianReflection : public Bxdf
    {
      public:
        LambertianReflection(const Spectrum& r)
            : Bxdf{BxdfKind::kReflection | BxdfKind::kDiffuse}, m_r{r}
        {}

        Spectrum Eval(const Vec3f& wo, const Vec3f& wi) const override;
        Spectrum SampledEval(const Vec3f& wo, Vec3f& wi,
                             const glm::vec2& sample,
                             float& pdf) const override;
        Spectrum Rho(const Vec3f& wo,
                     std::span<const glm::vec2> samples) override;

      private:
        const Spectrum m_r;
    };
} // namespace rrt
