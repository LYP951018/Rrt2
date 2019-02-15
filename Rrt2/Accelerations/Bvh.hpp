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
	//因为插入是有层数限制的，层数到顶了直接 push_back，会出现一个节点有多个 Geometry 的情况。
	std::vector<const GeometryBase*> geometries;
	BoundingBox boundingBox;
	//其实就是有木有 children。
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