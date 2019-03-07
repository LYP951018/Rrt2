#pragma once

#include <cstddef>
#include "Helpers.hpp"
#include "Vec3.hpp"

class Ray;

class BoundingBox
{
  public:
    Float4 corners[2];

    /* const Vec3f& GetMin() const { return corners[kMinIndex]; }
     const Vec3f& GetMax() const { return corners[kMaxIndex]; }*/
    // const Vec3f& GetCentroid() const;
    // static DirectX::XMVECTOR GetCentroidSimd(DirectX::XMVECTOR minVert, DirectX::XMVECTOR
    // maxVert);

    bool Hit(const Ray& ray, float tMin, float tMax) const;

  private:
    enum : std::size_t
    {
        kMinIndex,
        kMaxIndex
    };
};