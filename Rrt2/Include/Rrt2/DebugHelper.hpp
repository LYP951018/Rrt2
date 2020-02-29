#pragma once

#include "Simd.hpp"
#include <string>

namespace rrt
{
    std::string StringifyFloat4(Float4 v);
    std::string StringifyInt4(Int4 v);

    // `const Int4&` makes windbg happy
    void DumpIntsToStdOut(const Int4& v);
}

