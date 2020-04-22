#include "Rrt2/Geometries/TriangleMesh.hpp"
#include "Rrt2/PrimRef.hpp"
#include <algorithm>

namespace rrt
{
    std::unique_ptr<Vec3f[]> CreateVertices(std::uint32_t verticesCount)
    {
        return std::make_unique<Vec3f[]>(verticesCount);
    }


    TriangleMesh::TriangleMesh(std::uint32_t verticesCount,
                               std::vector<TriangleIndices> indices,
                               const Vec3f* positions, const Vec3f* normals,
                               const Vec3f* tangents)
        : m_verticesCount{verticesCount},
          // FillPrimitiveArray 会一次读四个，position 只有仨。
          // TODO: 要不要这里让每个 position alignas(16)？
          // struct alignas(16) Vec3f : glm::vec3 {};
          m_positions{CreateVertices(verticesCount + 1)},
          m_normals{CreateVertices(verticesCount)},
          m_tangents{CreateVertices(verticesCount)},
          m_indices{std::move(indices)}
    {
        CopyVertices(m_positions, positions);
        CopyVertices(m_normals, normals);
        if (tangents == nullptr)
        {
            CalculateTangentVecs();
        }
        else
        {
            CopyVertices(m_tangents, tangents);
        }
    }

    SingleTriangle TriangleMesh::GetPrimitiveAt(std::uint32_t id) const
    {
        const TriangleIndices& indices = m_indices[id];

        return SingleTriangle{m_positions[indices.v0], m_positions[indices.v1],
                              m_positions[indices.v2]};
    }

    void TriangleMesh::FillPrimitiveArray(AlignedVec<PrimRefStorage>& prims) const
    {
        std::uint32_t primId = 0;
        // TODO: loop unrolling
        for (const TriangleIndices& indices : m_indices)
        {
            const Float4 x0 = FloatsFromMemory(&m_positions[indices.v0].x);
            const Float4 x1 = FloatsFromMemory(&m_positions[indices.v1].x);
            const Float4 x2 = FloatsFromMemory(&m_positions[indices.v2].x);
            const Float4 lower = Min(x2, Min(x0, x1));
            const Float4 upper = Max(x2, Max(x0, x1));
            BoundingBox box{lower, upper};
            prims.push_back(PrimRefStorage{box, GetGeomId(), primId});
            ++primId;
        }
    }

    void TriangleMesh::CopyVertices(const std::unique_ptr<Vec3f[]>& dest,
                                    const Vec3f* src)
    {
        std::copy(src, src + m_verticesCount, dest.get());
    }

    void TriangleMesh::CalculateTangentVecs()
    {
    }
} // namespace rrt
