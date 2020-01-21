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

inline Vec3fPacked Cross3(const Vec3fPacked& lhs, const Vec3fPacked& rhs)
{
    const PackedFloats s1 = Sub(Mul(lhs.y, rhs.z), Mul(lhs.z, rhs.y));
    const PackedFloats s2 = Sub(Mul(lhs.z, rhs.x), Mul(lhs.x, rhs.z));
    const PackedFloats s3 = Sub(Mul(lhs.x, rhs.y), Mul(lhs.y, rhs.x));
    return Vec3fPacked{s1, s2, s3};
}

inline PackedFloats Dot3(const Vec3fPacked& lhs, const Vec3fPacked& rhs)
{
    const PackedFloats s0 = Mul(lhs.x, rhs.x);
    const PackedFloats s1 = Mul(lhs.y, rhs.y);
    const PackedFloats s2 = Mul(lhs.z, rhs.z);
    return Add(s0, Add(s1, s2));
}

inline Vec3fPacked DupPackedFloats(PackedFloats floats)
{
    return Vec3fPacked{PermuteFloats<0, 0, 0, 0>(floats), PermuteFloats<1, 1, 1, 1>(floats),
                       PermuteFloats<2, 2, 2, 2>(floats)};
}

inline Vec3fPacked DupPackedFloats(const Vec3f& vec3)
{
    return Vec3fPacked{
        MakeFloats(vec3.x),
        MakeFloats(vec3.y),
        MakeFloats(vec3.z),
    };
}

inline Vec3fPacked operator-(const Vec3fPacked& lhs, const Vec3fPacked& rhs)
{
    return Vec3fPacked{Sub(lhs.x, rhs.x), Sub(lhs.y, rhs.y), Sub(lhs.z, rhs.z)};
}

inline Vec3fPacked operator*(const Vec3fPacked& lhs, const Vec3fPacked& rhs)
{
    return Vec3fPacked{Mul(lhs.x, rhs.x), Mul(lhs.y, rhs.y), Mul(lhs.z, rhs.z)};
}

inline Vec3fPacked Scale(const Vec3fPacked& lhs, const PackedFloats& rhs)
{
    return Vec3fPacked{Mul(lhs.x, rhs), Mul(lhs.y, rhs), Mul(lhs.z, rhs)};
}

inline Vec3fPacked operator-(const Vec3fPacked& lhs)
{
    return Vec3fPacked{Neg(lhs.x), Neg(lhs.y), Neg(lhs.z)};
}

inline Vec3fPacked Rcp(const Vec3fPacked& lhs)
{
    return Vec3fPacked{Rcp(lhs.x), Rcp(lhs.y), Rcp(lhs.z)};
}

inline PackedFloats SelectMaxElement(const Vec3fPacked& vec)
{
    return Max(vec.x, Max(vec.y, vec.z));
}

inline PackedFloats SelectMinElement(const Vec3fPacked& vec)
{
    return Min(vec.x, Min(vec.y, vec.z));
}

#undef EXTERN_VEC