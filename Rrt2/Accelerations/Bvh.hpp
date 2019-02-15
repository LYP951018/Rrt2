#pragma once

#include "AccelerationBase.hpp"
#include "../BoundingBox.hpp"
#include <vector>
#include <gsl/span>

class GeometryBase;

struct OctTreeNode
{
	OctTreeNode();

	OctTreeNode* children[8];
	//��Ϊ�������в������Ƶģ�����������ֱ�� push_back�������һ���ڵ��ж�� Geometry �������
	std::vector<const GeometryBase*> geometries;
	BoundingBox boundingBox;
	//��ʵ������ľ�� children��
	bool IsLeaf;
};


struct OctTree
{
public:
	inline static constexpr std::uint32_t kMaxDepth = 16;

	OctTree(const BoundingBox& sceneBoundingBox);
	void Build();
	void Insert(const GeometryBase* geometry);
	const OctTreeNode* GetRoot() const { return m_root; }

private:
	//void Insert(OctTreeNode* node, const GeometryBase* geometry);
	void Build(OctTreeNode* node);
	void InsertSimd(OctTreeNode* node, const GeometryBase* geometry, std::uint32_t depth);

	OctTreeNode* m_root;
};

class Bvh : public AccelerationBase
{
public:
	std::optional<HitRecord> Hit(const Ray& ray, float tMin, float tMax) override;

private:
};