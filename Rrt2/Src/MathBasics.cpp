#include <Rrt2/MathBasics.hpp>
#include <Rrt2/StdExtras.hpp>
#include <bit>

namespace rrt
{
    bool AlmostEqualUlps(float lhs, float rhs, std::int32_t expectedUlps)
    {
        if (std::signbit(lhs) != std::signbit(rhs))
        {
            return false;
        }
        const std::int32_t lhsInteger = bit_cast<std::int32_t>(lhs);
        const std::int32_t rhsInteger = bit_cast<std::int32_t>(rhs);

        const std::int32_t ulps = std::abs(rhsInteger - lhsInteger);
        return ulps <= expectedUlps;
    }

    bool AlmostEqualAbsolute(float lhs, float rhs, float epsilon)
    {
        return std::abs(rhs - lhs) <= epsilon;
    }
} // namespace rrt
