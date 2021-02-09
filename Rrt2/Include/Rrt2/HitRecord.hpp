#pragma once

#include "Vec3.hpp"
#include <glm/vec2.hpp>
#include <vector>
#include <memory>

namespace rrt
{
    class GeometryBase;
    class Ray;
    class Bsdf;

    struct SurfaceInteraction
    {
        // 使用加速结构计算相交时必写的字段。

        // barycentric coord
        Vec2f triangleUV;
        float time;
        std::uint32_t primId;
        std::uint32_t geomId;

        // 在计算交叉时不会写下面的字段

        // 因为即使三角形 T1 和光线交叉于 t_1
        // 也可能有三角形 T2 和光线交叉于 t_2 
        // 并且 t_2 < t_1
        // 如果在 T1 就计算了 position，这个计算就是多余的了。
        Vec4f position;
        Vec4f shadingNormal;
        Vec4f geometyNormal;

        // 出射光线的方向，与命中物体时光的方向相反。
        Vec3f wo;
        Vec2f texCoord;
        Vec4f dpdu, dpdv;
        // 我们暂时不管浮点误差
        // pbrt 对浮点误差进行了比较严谨的处理
        // 计算交点时会将 pError 赋值为理论浮点误差最大值
        // 利用这个最大值调整最终的交点
        // Vec3f pError;

        std::unique_ptr<Bsdf> bsdf;

        Ray SpawnRay(const Vec3f& dir) const;
        Ray SpawnRayTo(const Vec3f& dst) const;
        Vec3f GetHitPos(const Ray& ray) const;
    };
}
