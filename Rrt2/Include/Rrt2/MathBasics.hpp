
#pragma once

#include <cmath>
#include <cstdint>
#include <glm/vec3.hpp>
#include <cmath>

namespace rrt
{
    inline constexpr float kPi = 3.14159265358979323846f;
    inline constexpr float kInvPi = 1.0f / kPi;
    inline constexpr float kNegInf = -INFINITY;
    inline constexpr float kInf = INFINITY;

    bool AlmostEqualUlps(float lhs, float rhs, std::int32_t ulps);

    template<int N>
    bool AlmostEqualUlps(const glm::vec<N, float, glm::highp>& lhs,
                         const glm::vec<N, float, glm::highp>& rhs,
                         std::int32_t ulps)
    {
        for (int i = 0; i < N; ++i)
        {
            if (!AlmostEqualUlps(lhs[i], rhs[i], ulps))
                return false;
        }
        return true;
    }

    bool AlmostEqualAbsolute(float lhs, float rhs, float epsilon = 1e-6f);

    template<int N>
    bool AlmostEqualAbsolute(const glm::vec<N, float, glm::highp>& lhs,
                             const glm::vec<N, float, glm::highp>& rhs,
                             float epsilon = 1e-6f)
    {
        for (int i = 0; i < N; ++i)
        {
            if (!AlmostEqualAbsolute(lhs[i], rhs[i], epsilon))
                return false;
        }
        return true;
    }

} // namespace rrt
