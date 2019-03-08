#pragma once

#include <DirectXMath.h>
#include "Vec3.hpp"

inline DirectX::XMVECTOR LoadFloat3(const float* f)
{
    return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(f));
}

inline DirectX::XMVECTOR LoadFloat3(const Vec3f& f)
{
    return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&f.x));
}

inline DirectX::XMVECTOR LoadFloat2(const float* f)
{
    return DirectX::XMLoadFloat2(reinterpret_cast<const DirectX::XMFLOAT2*>(f));
}