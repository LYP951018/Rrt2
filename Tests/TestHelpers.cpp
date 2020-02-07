#include "TestHelpers.hpp"

bool M128Comparer::operator== (const M128Comparer& rhs) const {
    return rrt::Msbs(rrt::Equal(value, rhs.value)) == 0xF;
}
