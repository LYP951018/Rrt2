#include "TestHelpers.hpp"
#include <Rrt2/DebugHelper.hpp>
#include <ostream>

using namespace rrt;

std::ostream& operator<<(std::ostream& os, const M128Comparer& value)
{
    os << StringifyFloat4(value.value);
    return os;
}

bool M128Comparer::operator==(const M128Comparer& rhs) const
{
    return rrt::Msbs(rrt::Equal(value, rhs.value)) == 0xF;
}
