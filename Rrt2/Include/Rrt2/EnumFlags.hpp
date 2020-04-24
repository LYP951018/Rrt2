#pragma once

#include <type_traits>

namespace rrt
{
#define RRT_DEF_OPERATOR(T, op)                                     \
    inline constexpr T operator op(T lhs, T rhs)                    \
    {                                                               \
        using Underlying = std::underlying_type_t<T>;               \
        return static_cast<T>(static_cast<Underlying>(lhs)          \
                                  op static_cast<Underlying>(rhs)); \
    }                                                               \
    inline constexpr T& operator op##=(T& lhs, T rhs)               \
    {                                                               \
        lhs = lhs op rhs;                                           \
        return lhs;                                                 \
    }

#define RRT_DEF_NOT_OPERATOR(T)                               \
    inline constexpr T operator~(T lhs)                       \
    {                                                         \
        using Underlying = std::underlying_type_t<T>;         \
        return static_cast<T>(~static_cast<Underlying>(lhs)); \
    }

#define RRT_DEF_FLAG_ENUM_OPS(type) \
    RRT_DEF_OPERATOR(type, |)       \
    RRT_DEF_OPERATOR(type, &)       \
    RRT_DEF_NOT_OPERATOR(type)
} // namespace rrt