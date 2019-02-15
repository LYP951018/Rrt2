#pragma once

#include "CommonDefs.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>

#define CONCAT_IMPL(A, B) A##B

#define CONCAT(A, B) CONCAT_IMPL(A, B)

#define DEF_GETTER(type, name, member) \
    type name() const { return member; }

inline DirectX::XMVECTOR Load(const DirectX::XMFLOAT3& f3) { return DirectX::XMLoadFloat3(&f3); }
inline void Store(DirectX::XMVECTOR vec, DirectX::XMFLOAT3& f3)
{
    DirectX::XMStoreFloat3(&f3, vec);
}

inline DirectX::XMFLOAT3 Store(DirectX::XMVECTOR vec)
{
    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, vec);
    return result;
}

inline float Float3At(const DirectX::XMFLOAT3& f3, std::uint32_t index)
{
    assert(index >= 0 && index < 3);
    return reinterpret_cast<const float*>(reinterpret_cast<const std::byte*>(&f3))[index];
}

inline float Dot(DirectX::XMVECTOR lhs, DirectX::XMVECTOR rhs)
{
	using namespace DirectX;
	return XMVectorGetX(XMVector3Dot(lhs, rhs));
}