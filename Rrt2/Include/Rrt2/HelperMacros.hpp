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

#if NDEBUG
#if _MSC_VER
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE [[gnu::always_inline]]
#endif
#else
#define ALWAYS_INLINE
#endif

#if _MSC_VER
#define RRT_UNREACHABLE __assume(0)
#endif