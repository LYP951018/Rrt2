#pragma once

#include "../Vec3.hpp"
#include "../HitRecord.hpp"
#include <optional>

class Scene;
class PrimRef;
class TriangleMesh;
class SingleTriangle;
class PackedRay;
class Ray;

class alignas(16) SimdTriangle
{
  public:
    Vec3fPacked v0;
    Vec3fPacked e1; // v1 - v0
    Vec3fPacked e2; // v2 - v0
    PackedInts primIds;
    PackedInts geomIds;

    static std::uint32_t GetSimdCount(std::uint32_t primCount)
    {
        return (primCount + kMaxSimdWidth - 1) / kMaxSimdWidth;
    }
    static std::uint32_t GetCapacity() { return kMaxSimdWidth; }

    void Set(std::uint32_t i, const SingleTriangle& triangle, std::uint32_t primId,
             std::uint32_t geomId);
    void Fill(const PrimRef* prims, std::uint32_t& start, std::uint32_t end, const Scene* scene);

    std::optional<HitRecord> Hit(const PackedRay& packedRay, const Ray& ray, float tMin,
                                 float tMax) const;
};