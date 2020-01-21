#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#define CONCAT_IMPL(A, B) A##B

#define CONCAT(A, B) CONCAT_IMPL(A, B)

#define DEF_GETTER(type, name, member) \
    type name() const { return member; }

#define SET_COPY(deleteOrDefault, type)  \
    type(const type&) = deleteOrDefault; \
    type& operator=(const type&) = deleteOrDefault;

#define SET_MOVE(deleteOrDefault, type) \
    type(type&&) = deleteOrDefault;     \
    type& operator=(type&&) = deleteOrDefault;

#define DEFAULT_COPY(type) SET_COPY(default, type)
#define DELETE_COPY(type) SET_COPY(delete, type)
#define DEFAULT_MOVE(type) SET_MOVE(default, type)
#define DELETE_MOVE(type) SET_MOVE(delete, type)

template<class To, class From>
typename std::enable_if<(sizeof(To) == sizeof(From)) && std::is_trivially_copyable<From>::value &&
                            std::is_trivial<To>::value,
                        // this implementation requires that To is trivially default constructible
                        To>::type
// constexpr support needs compiler magic
bit_cast(const From& src) noexcept
{
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}