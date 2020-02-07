#pragma once

#include <Rrt2/Vec3.hpp>
#include <catch2/catch.hpp>

//仅供测试代码使用

//bool operator==(const rrt::PackedFloats& lhs, const rrt::PackedFloats& rhs);
struct M128Comparer
{
    __m128 value;

    bool operator == (const M128Comparer& rhs) const;
};

#define CHECK_M128_EQ(lhs, rhs) CHECK(M128Comparer{(lhs)} == M128Comparer{(rhs)})