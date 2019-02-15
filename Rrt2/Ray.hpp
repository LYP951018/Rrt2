#pragma once

#include "CommonDefs.hpp"
#include <gsl/span>

class Ray
{
  public:
    Ray(const PointType& start, const PointType& speed)
        : m_origin{start}, m_speed{speed}, m_invSpeed{1.0f / speed.x, 1.0f / speed.y,
                                                     1.0f / speed.z},
          m_negSpeed{m_speed.x < 0.0f, m_speed.y < 0.0f, m_speed.z < 0.0f}
    {}

    PointType GetPosAt(float t);
    gsl::span<const bool, 3> GetNegSpeed() const { return gsl::span<const bool, 3>{m_negSpeed}; }
	const PointType& GetInvSpeed() const { return m_invSpeed; }
	const PointType& GetOrigin() const { return m_origin; }
	const PointType& GetSpeed() const { return m_speed; }

  private:
    PointType m_origin;
    PointType m_speed;
    PointType m_invSpeed;
    //根据光线方向的不同，选择测试的平面也不同。
	//表示光线的方向的三个维度的正负情况。
    bool m_negSpeed[3];
};