#pragma once

#include "GeometryBase.hpp"
#include "../BoundingBox.hpp"

class Material;

class Triangle : public GeometryBase
{
public:
	inline static constexpr std::size_t kVertexCount = 3;

	Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2)
		: m_positions{v0, v1, v2}
	{
		ReconstructBoundingBox();
	}

	std::optional<HitRecord> Hit(const Ray& ray, float tMin, float tMax) override;
	const BoundingBox& GetBoundingBox() const override;
	const Vec3& GetV0() const;
	const Vec3& GetV1() const;
	const Vec3& GetV2() const;

private:
	void ReconstructBoundingBox();

	PointType m_positions[kVertexCount];
	Vec2 m_uvs[kVertexCount];
	Material* m_material;
	BoundingBox m_boundingBox;
};