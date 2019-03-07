#pragma once

#include <pcg_variants.h>
#include <cstdint>
#include <vector>
#include "Vec3.hpp"

// class Ray;
//
// class Camera
//{
// public:
//	Camera(std::uint32_t filmWidth, std::uint32_t filmHeight, float radius, float distance,
//const Vec3f& center); 	void SetLookAt(const Vec3f& eye, const Vec3f& focus, const Vec3f& up);
//
//
//	Vec3f center;
//	DirectX::XMFLOAT3 uAxis, vAxis, wAxis;
//	DirectX::XMFLOAT3 filmLeftBottomCorner;
//	DirectX::XMFLOAT4X4 uvMatrix;
// private:
//	std::uint32_t m_filmWidth, m_filmHeight;
//	float m_radius;
//	float m_distance;
//
//};
//
// void GenRay(const Camera& camera, DirectX::XMVECTOR center, DirectX::XMVECTOR
// filmLeftBottomCorner, DirectX::XMVECTOR u, DirectX::XMVECTOR v, DirectX::XMVECTOR w, 	const Vec2f&
//pixelPos, const Vec2f& thinLensPos, Ray& ray);
