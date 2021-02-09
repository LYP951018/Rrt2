#pragma once

#include <memory_resource>
#include <vector>

namespace rrt
{
    // FIXME
    template<typename T>
    using AlignedVec = std::pmr::vector<T>;
}
