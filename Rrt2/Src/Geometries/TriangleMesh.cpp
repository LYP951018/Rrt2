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
                               std::vector<std::uint16_t> indices,
                               const Vec3f* positions, const Vec3f* normals,
                               const Vec3f* tangents)
        : m_verticesCount{verticesCount},
          // FillPrimitiveArray 会一次读四个，position 只有仨。
          // TODO: 要不要这里让每个 position alignas(16)？
          // struct alignas(16) Vec3f : glm::vec3 {};
          m_positions{CreateVertices(verticesCount + 1)},
          m_normals{CreateVertices(verticesCount + 1)},
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

    std::span<const Vec3f> TriangleMesh::GetNormals() const
    {
        return {m_normals.get(), static_cast<std::size_t>(m_verticesCount)};
    }

    std::span<const Vec3f> TriangleMesh::GetPositions() const
    {
        return {m_positions.get(), static_cast<std::size_t>(m_verticesCount)};
    }

    void TriangleMesh::FillPrimitiveArray(AlignedVec<PrimRefStorage>& prims) const
    {
        std::uint32_t primId = 0;
        // TODO: loop unrolling
        for (std::int32_t i = 0; i < m_indices.size(); i += 3)
        {
            const Float4 x0 = FloatsFromMemory(&m_positions[i].x);
            const Float4 x1 = FloatsFromMemory(&m_positions[i + 1].x);
            const Float4 x2 = FloatsFromMemory(&m_positions[i + 2].x);
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
