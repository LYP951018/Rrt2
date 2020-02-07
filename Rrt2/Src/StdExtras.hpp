#pragma once

#include <type_traits>
#include <cstring>

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
} // namespace rrt
