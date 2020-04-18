#pragma once

#include "../Vec3.hpp"
#include "../HitRecord.hpp"
#include "../StorageLoadStore.hpp"
#include <glm/vec4.hpp>
#include <optional>
#include <array>
#include <cassert>

namespace rrt
{
    class Scene;
    class PrimRefStorage;
    class TriangleMesh;
    struct SingleTriangle;
    class PackedRay;
    class Ray;

    class alignas(16) PackedTriangle
    {
      public:
        Vec3fPacked v0;
        Vec3fPacked e1; // v1 - v0
        Vec3fPacked e2; // v2 - v0
        std::array<int, 4> primIds;
        std::array<int, 4> geomIds;

        static std::uint32_t GetSimdCount(std::uint32_t primCount)
        {
            return (primCount + kMaxSimdWidth - 1) / kMaxSimdWidth;
        }
        static std::uint32_t GetCapacity() { return kMaxSimdWidth; }

        std::optional<SurfaceInteraction> Hit(const PackedRay& packedRay, const Ray& ray,
                                     float tMin, float tMax) const;
    };

    struct alignas(kSimdAlignment) PackedTriangleStorage
    {
        Vec3fPackedStorage v0;
        Vec3fPackedStorage e1;
        Vec3fPackedStorage e2;
        std::array<int, 4> primIds;
        std::array<int, 4> geomIds;

        void Set(std::uint32_t i, const SingleTriangle& triangle,
                 std::uint32_t primId, std::uint32_t geomId);
        void Fill(const PrimRefStorage* prims, std::uint32_t& start,
                  std::uint32_t end, const Scene* scene);

        PackedTriangle ALWAYS_INLINE Load() const
        {
            // FIXME: redundant copy
            return PackedTriangle {
                .v0 = LoadStorage(v0),
                .e1 = LoadStorage(e1),
                .e2 = LoadStorage(e2),
                .primIds = primIds,
                .geomIds = geomIds
            };
        }
    };


} // namespace rrt
