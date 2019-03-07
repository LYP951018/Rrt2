#pragma once

#include <gsl/span>
#include "Vec3.hpp"

class Ray
{
  public:
    Ray(const Vec3f& start, const Vec3f& speed)
        : m_origin{start}, m_speed{speed}, m_invSpeed{1.0f / speed.x, 1.0f / speed.y,
                                                      1.0f / speed.z},
          m_negSpeed{m_speed.x < 0.0f, m_speed.y < 0.0f, m_speed.z < 0.0f}
    {}

    Vec3f GetPosAt(float t);
    gsl::span<const bool, 3> GetNegSpeed() const { return gsl::span<const bool, 3>{m_negSpeed}; }
    const Vec3f& GetInvSpeed() const { return m_invSpeed; }
    const Vec3f& GetOrigin() const { return m_origin; }
    const Vec3f& GetSpeed() const { return m_speed; }

  private:
    Vec3fPacked m_origin;
    Vec3fPacked m_speed;
    Vec3fPacked m_invSpeed;
    //根据光线方向的不同，选择测试的平面也不同。
    //表示光线的方向的三个维度的正负情况。
    bool m_negSpeed[3];
};