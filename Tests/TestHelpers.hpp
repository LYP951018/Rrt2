#pragma once

#include <Rrt2/Vec3.hpp>
#include <catch2/catch.hpp>
#include <iosfwd>

//仅供测试代码使用
struct M128Comparer
{
    __m128 value;

    bool operator==(const M128Comparer& rhs) const;
};

std::ostream& operator<<(std::ostream& os, const M128Comparer& value);

#define CHECK_M128_EQ(lhs, rhs) \
    CHECK(M128Comparer{(lhs)} == M128Comparer{(rhs)})