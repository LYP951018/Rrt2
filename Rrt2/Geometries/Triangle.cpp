
//#include "../Ray.hpp"
//#include "../Helpers.hpp"
//#include <DirectXMath.h>
//
//#define ENABLE_SIMD 1
//
// using namespace DirectX;
//
////ray.GetOrigin() + t * ray.GetSpeed() = m_positions[0] + β(m_positions[1] - m_positions[0]) +
///γ(m_positions[2] - m_positions[0])
////https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
// std::optional<HitRecord> TriangleMesh::Hit(const Ray & ray, float tMin, float tMax)
//{
//#if defined(ENABLE_SIMD)
//
//	const auto loadPos = [this](std::size_t index) -> XMVECTOR {
//		return m_positions[index].Load();
//	};
//	const XMVECTOR rayDir = ray.GetSpeed().Load();
//	const XMVECTOR v0 = loadPos(0);
//	const XMVECTOR v1 = loadPos(1);
//	const XMVECTOR v2 = loadPos(2);
//	const XMVECTOR v0v1 = v1 - v0;
//	const XMVECTOR v0v2 = v2 - v0;
//	const XMVECTOR pVec = XMVector3Cross(rayDir, v0v2);
//	const float det = Dot(v0v1, pVec);
//	const float invDet = 1.0f / det;
//	const XMVECTOR tVec = ray.GetOrigin().Load() - v0;
//	const float u = Dot(tVec, pVec);
//	const XMVECTOR qVec = XMVector3Cross(tVec, v0v1);
//	const float v = Dot(rayDir, qVec) * invDet;
//	const float t = Dot(v0v2, qVec) * invDet;
//#else
//	assert(false);
//#endif
//	return std::optional<HitRecord>();
//}
