#pragma once

#include <cstddef>
#include "Helpers.hpp"

class Ray;

class BoundingBox
{
  public:
    BoundingBox(const PointType& min, const PointType& max) : m_corners{min, max} {}
	BoundingBox();

    const PointType& GetMin() const { return m_corners[kMinIndex]; }
    const PointType& GetMax() const { return m_corners[kMaxIndex]; }
	const Vec3& GetCentroid() const;
	static DirectX::XMVECTOR GetCentroidSimd(DirectX::XMVECTOR minVert, DirectX::XMVECTOR maxVert);

    bool Hit(const Ray& ray, float tMin, float tMax) const;

  private:
    enum : std::size_t
    {
        kMinIndex,
        kMaxIndex
    };

    PointType m_corners[2];
};