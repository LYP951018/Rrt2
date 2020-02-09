#pragma once

#include <xmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <cassert>
#include <cstdint>
#include <gsl/gsl_assert>
#include "StdExtras.hpp"
#include "Helpers.hpp"

#define DefConstInlineVar(type, name, ...) inline const type name = __VA_ARGS__

#ifdef _WIN32
#define VECTORCALL __vectorcall
#else
#define VECTORCALL
#endif

namespace rrt
{
    inline constexpr std::uint32_t kMaxSimdWidth = 4;

    template<int N>
    struct SimdTraits
    {
        static_assert(N == 4 || N == 8);
    };

    template<>
    struct SimdTraits<4>
    {
        using FloatType = __m128;
        using IntType = __m128i;
        static inline constexpr std::size_t Alignment = 16;
    };

    template<>
    struct SimdTraits<8>
    {
        using FloatType = __m256;
        using IntType = __m256i;
        static inline constexpr std::size_t Alignment = 32;
    };

    template<int N>
    using Floats = typename SimdTraits<N>::FloatType;

    template<int N>
    using Ints = typename SimdTraits<N>::IntType;

    using Float4 = Floats<4>;
    using PackedFloats = Float4;
    using Int4 = Ints<4>;
    using PackedInts = Int4;
    inline constexpr std::size_t kSimdAlignment = SimdTraits<4>::Alignment;
    using Doubles = __m128d;

    template<typename T>
    void EnsureAligned(T* p)
    {
        Ensures((reinterpret_cast<std::uintptr_t>(p) & 0x7u) == 0);
    }

    inline Float4 MakeFloats(float x, float y, float z, float w) noexcept
    {
        return _mm_set_ps(w, z, y, x);
    }

    inline Int4 MakeInts(std::int32_t x) noexcept { return _mm_set1_epi32(x); }

    inline Int4 MakeInts(std::int32_t x, std::int32_t y, std::int32_t z,
                         std::int32_t w) noexcept
    {
        return _mm_set_epi32(w, z, y, x);
    }

    inline Float4 MakeFloats(float x) noexcept { return _mm_set_ps1(x); }

    inline Doubles MakeDoubles(double x, double y) noexcept
    {
        return _mm_set_pd(x, y);
    }

    template<std::uint32_t u0, std::uint32_t u1, std::uint32_t u2,
             std::uint32_t u3>
    inline Float4 VECTORCALL PermuteFloats(Float4 floats) noexcept
    {
        static_assert(u0 <= 3 && u1 <= 3 && u2 <= 3 && u3 <= 3,
                      "Indexes should be less or equal than 3.");
        return _mm_permute_ps(floats, static_cast<int /*std::int32_t*/>(
                                          u3 << 6 | u2 << 4 | u1 << 2 | u0));
    }

    inline Float4 VECTORCALL DupX(Float4 floats) noexcept
    {
        return PermuteFloats<0, 0, 0, 0>(floats);
    }

    inline Float4 VECTORCALL DupY(Float4 floats) noexcept
    {
        return PermuteFloats<1, 1, 1, 1>(floats);
    }

    inline Float4 VECTORCALL DupZ(Float4 floats) noexcept
    {
        return PermuteFloats<2, 2, 2, 2>(floats);
    }

    inline Float4 VECTORCALL DupW(Float4 floats) noexcept
    {
        return PermuteFloats<3, 3, 3, 3>(floats);
    }

    inline Float4 AsFloats(Int4 ints) noexcept
    {
        return _mm_castsi128_ps(ints);
    }

    inline Float4 ZeroFloats() noexcept { return _mm_setzero_ps(); }

    inline Doubles ZeroDoubles() noexcept { return _mm_setzero_pd(); }

    DefConstInlineVar(Float4, UintToFloatBias,
                      MakeFloats(32768.0f * 65536.0f, 32768.0f * 65536.0f,
                                 32768.0f * 65536.0f, 32768.0f * 65536.0f));

    DefConstInlineVar(Float4, ZeroedFloats, ZeroFloats());

    DefConstInlineVar(Doubles, ZeroedDoubles, ZeroDoubles());

    inline void Store(Float4 floats, float* p) { _mm_storeu_ps(p, floats); }

    inline void StoreAligned(Float4 floats, float* p)
    {
        EnsureAligned(p);
        _mm_store_ps(p, floats);
    }

    inline float First(Float4 floats) noexcept { return _mm_cvtss_f32(floats); }

    inline float Second(Float4 floats) noexcept
    {
        return First(PermuteFloats<1, 0, 0, 0>(floats));
    }

    inline float Third(Float4 floats)
    {
        return First(PermuteFloats<2, 0, 0, 0>(floats));
    }

    inline float Fourth(Float4 floats) noexcept
    {
        return First(PermuteFloats<3, 0, 0, 0>(floats));
    }

    inline std::int32_t First(Int4 ints) noexcept
    {
        return bit_cast<std::int32_t>(First(AsFloats(ints)));
    }

    inline Float4 SetFirst(Float4 floats, float f) noexcept
    {
        return _mm_move_ss(floats, _mm_set_ss(f));
    }

    inline Float4 SetSecond(Float4 floats, float f)
    {
        const auto temp = MakeFloats(f);
        return _mm_insert_ps(floats, temp, 0x10);
    }

    inline Float4 SetThird(Float4 floats, float f) noexcept
    {
        const auto temp = MakeFloats(f);
        return _mm_insert_ps(floats, temp, 0x20);
    }

    inline Float4 SetFourth(Float4 floats, float f) noexcept
    {
        const auto temp = MakeFloats(f);
        return _mm_insert_ps(floats, temp, 0x30);
    }

    inline Float4 Min(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_min_ps(lhs, rhs);
    }

    inline Float4 Max(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_max_ps(lhs, rhs);
    }

    inline Float4 Less(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_cmple_ps(lhs, rhs);
    }

    inline Float4 Greater(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_cmpge_ps(lhs, rhs);
    }

    inline Float4 Equal(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_cmpeq_ps(lhs, rhs);
    }

    inline Float4 NotEqual(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_cmpneq_ps(lhs, rhs);
    }

    inline std::uint8_t VECTORCALL Msbs(Float4 floats) noexcept
    {
        return static_cast<std::uint8_t>(_mm_movemask_ps(floats));
    }

    inline Float4 VECTORCALL Add(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_add_ps(lhs, rhs);
    }

    inline Doubles VECTORCALL Add(Doubles lhs, Doubles rhs) noexcept
    {
        return _mm_add_pd(lhs, rhs);
    }

    inline Float4 VECTORCALL Sub(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_sub_ps(lhs, rhs);
    }

    inline Doubles VECTORCALL Sub(Doubles lhs, Doubles rhs) noexcept
    {
        return _mm_sub_pd(lhs, rhs);
    }

    inline Float4 VECTORCALL Mul(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_mul_ps(lhs, rhs);
    }

    inline Float4 VECTORCALL Scale(Float4 lhs, float scale) noexcept
    {
        return Mul(MakeFloats(scale), lhs);
    }

    inline Doubles VECTORCALL Mul(Doubles lhs, Doubles rhs) noexcept
    {
        return _mm_mul_pd(lhs, rhs);
    }

    inline Float4 VECTORCALL Div(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_div_ps(lhs, rhs);
    }

    inline Float4 VECTORCALL Rcp(Float4 vec) noexcept
    {
        return _mm_rcp_ps(vec);
    }

    inline Float4 VECTORCALL Sqrt(Float4 floats) noexcept
    {
        return _mm_sqrt_ps(floats);
    }

    inline Float4 VECTORCALL Neg(Float4 floats) noexcept
    {
        return Sub(ZeroFloats(), floats);
    }

    inline Float4 VECTORCALL And(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_and_ps(lhs, rhs);
    }

    inline Doubles VECTORCALL And(Doubles lhs, Doubles rhs) noexcept
    {
        return _mm_and_pd(lhs, rhs);
    }

    inline Float4 VECTORCALL NotAnd(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_andnot_ps(lhs, rhs);
    }

    inline Float4 VECTORCALL Or(Float4 lhs, Float4 rhs) noexcept
    {
        return _mm_or_ps(lhs, rhs);
    }

    namespace Internal
    {
        inline constexpr std::uint8_t CastToUint8(bool b) noexcept
        {
            return b ? 1 : 0;
        }
    } // namespace Internal

    inline Float4 FloatsFromMemory(const float* p) noexcept
    {
        return _mm_loadu_ps(p);
    }

    inline Float4 FloatsFromAlignedMemory(const float* p) noexcept
    {
        return _mm_load_ps(p);
    }

    inline Float4 GetInfinity() noexcept
    {
        return MakeFloats(bit_cast<float>(0x7F800000u));
    }

    inline Float4 GetNegInfinity() noexcept
    {
        return MakeFloats(bit_cast<float>(0xFF800000u));
    }

    inline Float4 GetOnes() noexcept
    {
        return MakeFloats(bit_cast<float>(0xFFFFFFFFu));
    }

    inline Float4 SelectMinElement(Float4 f)
    {
        const Float4 min1 = Min(f, PermuteFloats<1, 0, 3, 2>(f));
        const Float4 min2 = Min(min1, PermuteFloats<2, 2, 0, 0>(min1));
        return min2;
    }

    inline Float4 SelectMaxElement(Float4 f)
    {
        const Float4 max1 = Max(f, PermuteFloats<1, 0, 3, 2>(f));
        const Float4 max2 = Max(max1, PermuteFloats<2, 2, 0, 0>(max1));
        return max2;
    }

    template<std::uint8_t V0, std::uint8_t V1, std::uint8_t V2, std::uint8_t V3>
    inline Float4 SelectCombine(Float4 lhs, Float4 rhs)
    {
        constexpr std::uint8_t selector =
            V0 + (V1 << 1) + (V2 << 2) + (V3 << 3);
        return _mm_blend_ps(lhs, rhs, selector);
    }

    inline bool IsZero(Float4 value)
    {
        return Msbs(Equal(value, _mm_setzero_ps())) == 0xF;
    }

} // namespace rrt