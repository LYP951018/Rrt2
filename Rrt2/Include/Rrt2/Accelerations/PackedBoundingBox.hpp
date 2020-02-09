#pragma once

#include "../BoundingBox.hpp"
#include "../StorageLoadStore.hpp"
#include "../Vec3.hpp"

namespace rrt
{
    class BoundingBox;
    class PackedRay;

    class PackedBoundingBox
    {
      public:
        Vec3fPacked lower;
        Vec3fPacked upper;

        std::uint32_t Hit(const PackedRay& ray, float tMin, float tMax) const;

      private:
        const Vec3fPacked& Get(std::size_t n) const;
    };

    class PackedBoundingBoxStorage
    {
      public:
        Vec3fPackedStorage lower;
        Vec3fPackedStorage upper;

        inline PackedBoundingBox ALWAYS_INLINE Load() const
        {
            return PackedBoundingBox {
                .lower = LoadStorage(lower),
                .upper = LoadStorage(upper)
            };
        }

        void Set(std::uint32_t i, const BoundingBoxStorage& bbox);\
    };
}
