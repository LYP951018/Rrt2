#include "DebugHelper.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fmt/format.h>
#include <cstdio>

namespace rrt
{
    std::string StringifyFloat4(Float4 v)
    {
        alignas(kSimdAlignment) glm::vec4 loaded;
        StoreAligned(v, glm::value_ptr(loaded));
        return fmt::format("[{},{},{},{}]", loaded.x, loaded.y, loaded.z,
                           loaded.w);
    }

    std::string StringifyInt4(Int4 v)
    {
        alignas(kSimdAlignment) glm::ivec4 loaded;
        StoreAligned(v, glm::value_ptr(loaded));
        return fmt::format("[{},{},{},{}]", loaded.x, loaded.y, loaded.z,
                           loaded.w);
    }

    void DumpIntsToStdOut(const rrt::Int4& v)
    {
        std::puts(StringifyInt4(v).c_str());
    }
} // namespace rrt
