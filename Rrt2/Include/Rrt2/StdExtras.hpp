#pragma once

#include <type_traits>
#include <cstring>
#include <array>

namespace rrt
{
    // https://en.cppreference.com/w/cpp/numeric/bit_cast
    template<class To, class From>
    typename std::enable_if<(sizeof(To) == sizeof(From)) &&
                                std::is_trivially_copyable<From>::value &&
                                std::is_trivial<To>::value,
                            To>::type
    // constexpr support needs compiler magic
    bit_cast(const From& src) noexcept
    {
        To dst;
        std::memcpy(&dst, &src, sizeof(To));
        return dst;
    }

    template<typename T, std::size_t N1, std::size_t N2>
    using MDArray = std::array<std::array<T, N2>, N1>;
} // namespace rrt
