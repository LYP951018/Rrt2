#include "TriangleMesh.hpp"
#include "../PrimRef.hpp"

TriangleMesh::TriangleMesh(std::vector<Vec3f> positions, std::vector<TriangleIndices> indices)
    : m_positions{std::move(positions)}, m_indices{std::move(indices)}
{}

SingleTriangle TriangleMesh::GetPrimitiveAt(std::uint32_t id) const
{
    const TriangleIndices& indices = m_indices[id];

    return SingleTriangle{m_positions[indices.x], m_positions[indices.y], m_positions[indices.z]};
}

void TriangleMesh::FillPrimitiveArray(AlignedVec<PrimRef>& prims) const
{
    std::uint32_t primId = 0;
    for (const TriangleIndices& indices : m_indices)
    {
        const Float4 x0 = FloatsFromMemory(&m_positions[indices.x].x);
        const Float4 x1 = FloatsFromMemory(&m_positions[indices.y].x);
        const Float4 x2 = FloatsFromMemory(&m_positions[indices.z].x);
        const Float4 lower = Min(x2, Min(x0, x1));
        const Float4 upper = Max(x2, Max(x0, x1));
        BoundingBox box{lower, upper};
        prims.push_back(PrimRef{box, GetGeomId(), primId});
        ++primId;
    }
}