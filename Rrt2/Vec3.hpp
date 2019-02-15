#pragma once

#include <cstddef>
#include <cassert>
#include <gsl/span>
#include <DirectXMath.h>
#include <iosfwd>

#define VEC_IMPL(extent)                                                                        \
    inline static constexpr std::size_t kExtent = extent;                                       \
    gsl::span<const float, kExtent> AsArray() const                                             \
    {                                                                                           \
        return gsl::span<const float, kExtent>{                                                 \
            reinterpret_cast<const float*>(reinterpret_cast<const std::byte*>(this)), static_cast<std::ptrdiff_t>(kExtent)}; \
    }                                                                                           \
    gsl::span<float, kExtent> AsArray()                                                         \
    {                                                                                           \
        return gsl::span<float, kExtent>{                                                       \
            reinterpret_cast<float*>(reinterpret_cast<std::byte*>(this)), static_cast<std::ptrdiff_t>(kExtent)};             \
    }                                                                                           \
    float operator[](std::size_t n) const                                                       \
    {                                                                                           \
        assert(n < kExtent);                                                                    \
        return AsArray()[n];                                                                    \
    }                                                                                           \
    float& operator[](std::size_t n)                                                            \
    {                                                                                           \
        assert(n < kExtent);                                                                    \
        return AsArray()[n];                                                                    \
    }                                                                                           \
    DirectX::XMVECTOR Load() const                                                              \
    {                                                                                           \
        const DirectX::XMFLOAT4 f4 = IntoFloat4();                                              \
        return DirectX::XMLoadFloat4(&f4);                                                      \
    }

class Vec2
{
public:
	float x, y;

	DirectX::XMFLOAT4 IntoFloat4() const
	{
		return { x, y, 0.0f, 0.0f };
	}

	VEC_IMPL(2)
};

class Vec3
{
public:
	float x, y, z;

	Vec3() = default;

	explicit Vec3(float v)
		: x{ v }, y{v}, z{v}
	{}

	Vec3(float x_, float y_, float z_)
		: x{x_}, y{y_}, z{z_}
	{}

	VEC_IMPL(3)

	DirectX::XMFLOAT4 IntoFloat4() const
	{
		return { x, y, z, 0.0f };
	}

	static Vec3 FromVec(DirectX::XMVECTOR vec);
};

std::ostream& operator << (std::ostream& os, const Vec3& value);