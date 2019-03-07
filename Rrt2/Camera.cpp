#include "Camera.hpp"
#include "Sampling.hpp"

// using namespace DirectX;

// Camera::Camera(std::uint32_t filmWidth, std::uint32_t filmHeight, float radius, float distance,
// const Vec3f& center) 	: m_filmWidth{filmWidth}, 	m_filmHeight{filmHeight}, 	m_radius{radius},
//	m_distance{distance},
//	center{center},
//	uAxis{1.0f, 0.0f, 0.0f},
//	vAxis{0.0f, 1.0f, 0.0f},
//	wAxis{0.0f, 0.0f, 1.0f}
//{
//}
//
// void GenRay(const Camera& camera, DirectX::XMVECTOR center, DirectX::XMVECTOR
// filmLeftBottomCorner, 	const XMMATRIX& uvMatrix, const Vec2f & pixelPos, const Vec2f & thinLensPos,
//Ray & ray)
//{
//	const XMVECTOR loadedThinLensPos = thinLensPos.Load();
//	const XMVECTOR origin = XMVector2Transform((loadedThinLensPos - XMVectorSet(0.5f, 0.5f,
//0.0f, 0.0f)) + center, uvMatrix); 	const XMVECTOR target = pixelPos.Load() + filmLeftBottomCorner;
//	const XMVECTOR dir = XMVector3Normalize(target - origin);
//	ray = Ray{}
//}
