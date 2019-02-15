#include "BoundingBox.hpp"
#include "Ray.hpp"
#include <gsl/span>

BoundingBox::BoundingBox()
{
}

DirectX::XMVECTOR BoundingBox::GetCentroidSimd(DirectX::XMVECTOR minVert, DirectX::XMVECTOR maxVert)
{
	return DirectX::XMVectorScale(DirectX::XMVectorAdd(minVert, maxVert), 0.5f);
}

//https://tavianator.com/fast-branchless-raybounding-box-intersections-part-2-nans/
bool BoundingBox::Hit(const Ray& ray, float tMin, float tMax) const
{
    const gsl::span<const bool, 3> negs = ray.GetNegSpeed();
	const PointType& invSpeed = ray.GetInvSpeed();
	//第一个索引：在平行的两个面中选一个
	//第二个索引：x, y, z
	//这里不处理 origin 和 AABB 某个顶点的某个维度相等，并且这个维度的 speed 为 0 的问题：
	//speed 为 0，invSpeed 为 +-∞，origin 的某个维度（x, y, z）与 AABB 相减后，为 0，0 * ∞ == NaN。
	for (unsigned i = 0; i < 3; ++i)
	{
		const float t0 = (m_corners[negs[i]][i] - ray.GetOrigin()[i]) * invSpeed[i];
		const float t1 = (m_corners[1 - negs[i]][i] - ray.GetOrigin()[i]) * invSpeed[i];
		tMin = std::max(t0, tMin);
		tMax = std::min(t1, tMax);
		if (tMin >= tMax) return false;
	}
	return true;
}
