#pragma once

#include <cstdint>
#include <glm/gtc/type_ptr.hpp>
#include "Helpers.hpp"
#include "Vec3.hpp"
#include "Simd.hpp"

namespace rrt
{
    //  (x, y, z), (x, y, z), (x, y, z), (x, y, z) =>   x, x, x, x
    //                                                  y, y, y, y
    //                                                  z, z, z, z
    void Store(std::uint32_t i, Vec3fPackedStorage& packed, const Vec3f& vec3);

    inline Float4 ALWAYS_INLINE LoadStorage(const glm::vec4& storage)
    {
        return FloatsFromAlignedMemory(glm::value_ptr(storage));
    }

    inline Vec3fPacked ALWAYS_INLINE LoadStorage(const Vec3fPackedStorage& storage)
    {
        return Vec3fPacked {
            .x = LoadStorage(storage.x),
            .y = LoadStorage(storage.y),
            .z = LoadStorage(storage.z)
        };
    }

} // namespace rrt
