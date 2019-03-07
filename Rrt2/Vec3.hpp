#pragma once

#include <cstddef>
#include <cassert>
#include <cstdint>
#include "Simd.hpp"
#include <iosfwd>

#define EXTERN_VEC(type)                   \
    extern template class type<Floats<4>>; \
    extern template class type<Floats<8>>; \
    extern template class type<float>;

#define VEC_IMPL(extent)                                  \
    inline static constexpr std::size_t kExtent = extent; \
    const auto& operator[](std::size_t n) const           \
    {                                                     \
        assert(n < kExtent);                              \
        return (&x)[n];                                   \
    }                                                     \
    auto& operator[](std::size_t n)                       \
    {                                                     \
        assert(n < kExtent);                              \
        return (&x)[n];                                   \
    }

template<typename T>
class Vec2T
{
  public:
    T x;
    T y;

    VEC_IMPL(2)
};

EXTERN_VEC(Vec2T)

// template<int N>
// using Vec2fPacked = Vec2T<Floats<N>>;

using Vec2f = Vec2T<float>;
using Vec2fPacked = Vec2T<Floats<kMaxSimdWidth>>;

template<typename T>
class Vec3T
{
  public:
    T x;
    T y;
    T z;

    VEC_IMPL(3)
};

EXTERN_VEC(Vec3T)

using Vec3f = Vec3T<float>;
using Vec3fPacked = Vec3T<Floats<kMaxSimdWidth>>;

std::ostream& operator<<(std::ostream& os, const Vec3f& value);

#undef EXTERN_VEC