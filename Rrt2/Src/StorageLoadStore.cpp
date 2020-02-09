#include <Rrt2/StorageLoadStore.hpp>

namespace rrt
{
    void Store(std::uint32_t i, Vec3fPackedStorage& packed, const Vec3f& vec3)
    {
        for (std::uint32_t j = 0; j < 3; ++j)
        {
            packed[j][i] = vec3[j];
        }
    }
}
