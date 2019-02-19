#include "Bvh.hpp"
#include "../Geometries/GeometryBase.hpp"

OctTreeNode::OctTreeNode()
	: children{},
	IsLeaf(true)
{
}

OctTree::OctTree(const BoundingBox& sceneBoundingBox)
{
	m_root = std::make_unique<OctTreeNode>();
	m_root->boundingBox = sceneBoundingBox;
}

void OctTree::Build()
{
	//TODO: 这里可以不用递归，用层序遍历出一个 vector，从后向前处理。
	Build(m_root.get());
}

//因为只要物体的中心在 bounding box 里面，就算它在该节点下，Insert 后的
//Bounding box 可能没有完全覆盖该物体，所以需要 build 这一步来更新。
void OctTree::Build(OctTreeNode* node)
{
	using namespace DirectX;
	XMVECTOR minVert = g_XMNegInfinity, maxVert = g_XMInfinity;
	if (node->IsLeaf)
	{
		for (const GeometryBase* const geometry : node->geometries)
		{
			const BoundingBox& boundingBox = geometry->GetBoundingBox();
			minVert = XMVectorMax(minVert, boundingBox.GetMin().Load());
			maxVert = XMVectorMin(maxVert, boundingBox.GetMax().Load());
		}
	}
	else
	{
		for (const std::unique_ptr<OctTreeNode>& child : node->children)
		{
			if (child == nullptr) continue;
			Build(child.get());
			const BoundingBox& boundingBox = child->boundingBox;
			minVert = XMVectorMax(minVert, boundingBox.GetMin().Load());
			maxVert = XMVectorMin(maxVert, boundingBox.GetMax().Load());
		}
	}
	node->boundingBox = BoundingBox(Vec3::FromVec(minVert), Vec3::FromVec(maxVert));
}

void OctTree::Insert(const GeometryBase* geometry)
{
	InsertSimd(m_root.get(), geometry, 1);
}

void OctTree::InsertSimd(OctTreeNode* node, const GeometryBase* geometry, std::uint32_t depth)
{
	if (node->IsLeaf)
	{
		if (node->geometries.size() == 0 || depth == kMaxDepth)
		{
			node->geometries.push_back(geometry);
		}
		else
		{
			//已经有了一个对象，又要插入一个，此时要分裂。
			assert(node->geometries.size() == 1);
			node->IsLeaf = false;
			const GeometryBase* const oldGeometry = node->geometries[0];
			node->geometries.pop_back();
			InsertSimd(node, oldGeometry, depth + 1);
			InsertSimd(node, geometry, depth + 1);
		}
	}
	else
	{
		using namespace DirectX;
		//把 node 分成 8 个 cube
		const BoundingBox& cube = node->boundingBox;
		const XMVECTOR nodeMin = cube.GetMin().Load();
		const XMVECTOR nodeMax = cube.GetMax().Load();
		const XMVECTOR nodeCenter = BoundingBox::GetCentroidSimd(nodeMin, nodeMax);
		const BoundingBox& geometryBox = geometry->GetBoundingBox();
		const XMVECTOR geometryMin = geometryBox.GetMin().Load();
		const XMVECTOR geometryMax = geometryBox.GetMax().Load();
		const XMVECTOR geometryCenter = BoundingBox::GetCentroidSimd(geometryMin, geometryMax);
		const XMVECTOR comparation = XMVectorGreater(geometryCenter, nodeCenter);
		const int childIndex = _mm_movemask_ps(comparation) & 0b111;
		const XMVECTOR childBoxMin = XMVectorSelect(nodeCenter, geometryCenter, comparation);
		const XMVECTOR length = XMVectorScale(XMVectorSubtract(nodeMax, nodeMin), 0.5f);
		const XMVECTOR childBoxMax = XMVectorAdd(childBoxMin, length);
		
		std::unique_ptr<OctTreeNode>& child = node->children[childIndex];
		if (child == nullptr)
		{
			child = std::make_unique<OctTreeNode>();
			child->boundingBox = BoundingBox(Vec3::FromVec(childBoxMin), Vec3::FromVec(childBoxMax));
		}
		InsertSimd(child.get(), geometry, depth + 1);
	}
}



//能调用到这个函数就说明 geometry 保证在 node 里面。
// 留着为了 benchmark
//void OctTree::Insert(OctTreeNode* node, const GeometryBase * geometry)
//{
//	if (!node->IsLeaf())
//	{
//		node->geometry = geometry;
//	}
//	else
//	{
//		//把 node 分成 8 个 cube
//		const BoundingBox& cube = node->boundingBox;
//		const Vec3& minVert = cube.GetMin();
//		const Vec3& maxVert = cube.GetMax();
//		const float deltaX = (maxVert.x - minVert.x) / 2.0f;
//		const BoundingBox& geometryBox = geometry->GetBoundingBox();
//		const Vec3& geometryBoxCenter = geometryBox.GetCentroid();
//		const Vec3& nodeBoxCenter = cube.GetCentroid();
//		std::size_t childIndex = 0;
//		Vec3 childBoxMin, childBoxMax;
//		if (geometryBoxCenter.x > nodeBoxCenter.x)
//		{
//			childIndex = 4;
//			childBoxMin.x = nodeBoxCenter.x;
//		}
//		else
//		{
//			childBoxMin.x = minVert.x;
//		}
//		if (geometryBoxCenter.y > nodeBoxCenter.y)
//		{
//			childIndex += 2;
//			childBoxMin.y = nodeBoxCenter.y;
//		}
//		else
//		{
//			childBoxMin.y = minVert.y;
//		}
//		if (geometryBoxCenter.z > nodeBoxCenter.z)
//		{
//			childIndex += 1;
//			childBoxMin.z = nodeBoxCenter.z;
//		}
//		else
//		{
//			childBoxMin.z = minVert.z;
//		}
//		childBoxMax.x = childBoxMin.x + deltaX;
//		childBoxMax.y = childBoxMin.y + deltaX;
//		childBoxMax.z = childBoxMin.z + deltaX;
//
//		OctTreeNode*& child = node->children[childIndex];
//		if (child == nullptr)
//		{
//			std::unique_ptr<OctTreeNode> childNode = std::make_unique<OctTreeNode>();
//			childNode->boundingBox = BoundingBox(childBoxMin, childBoxMax);
//			child = childNode.release();
//		}
//		Insert(child, geometry);
//	}
//}