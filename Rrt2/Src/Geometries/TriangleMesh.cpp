#include "Rrt2/Geometries/TriangleMesh.hpp"
#include "Rrt2/PrimRef.hpp"
#include <array>
#include <algorithm>

namespace rrt
{
    std::unique_ptr<Vec3f[]> CreateVertices(std::uint32_t verticesCount)
    {
        return std::make_unique<Vec3f[]>(verticesCount);
    }

    TriangleMesh::TriangleMesh(std::uint32_t verticesCount,
                               std::vector<std::uint16_t> indices,
                               const Vec3f* positions, const Vec2f* texCoords,
                               const Vec3f* normals, const Vec3f* tangents)
        : m_verticesCount{verticesCount},
          // FillPrimitiveArray 会一次读四个，position 只有仨。
          // TODO: 要不要这里让每个 position alignas(16)？
          // struct alignas(16) Vec3f : glm::vec3 {};
          m_positions{CreateVertices(verticesCount + 1)},
          m_texCoords{std::make_unique<Vec2f[]>(verticesCount)},
          m_normals{CreateVertices(verticesCount + 1)},
          m_tangents{CreateVertices(verticesCount + 1)},
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

    std::span<const Vec3f> TriangleMesh::GetTangents() const
    {
        return {m_normals.get(), static_cast<std::size_t>(m_verticesCount)};
    }

    std::span<const Vec2f> TriangleMesh::GetTexCoords() const
    {
        return { m_texCoords.get(), static_cast<std::size_t>(m_verticesCount) };
    }

    void
    TriangleMesh::FillPrimitiveArray(AlignedVec<PrimRefStorage>& prims) const
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

    void TriangleMesh::Interpolate(SurfaceInteraction& interaction) const
    {
        const std::span<const Vec3f> normals = GetNormals();
        const std::uint32_t primId = interaction.primId;
        InterpolateStream(normals, primId, interaction.triangleUV,
                          interaction.shadingNormal);
        InterpolateStream(GetTangents(), primId, interaction.triangleUV,
                          interaction.dpdu);
        InterpolateTexCoord(primId, interaction.triangleUV, interaction.texCoord);
        CalcGeometryNormal(primId, interaction.geometyNormal);
    }

    void TriangleMesh::CopyVertices(const std::unique_ptr<Vec3f[]>& dest,
                                    const Vec3f* src)
    {
        std::copy(src, src + m_verticesCount, dest.get());
    }

    void TriangleMesh::CalculateTangentVecs() {}

    void TriangleMesh::InteroplateDpDu() {}

    void TriangleMesh::InterpolateStream(std::span<const Vec3f> stream,
                                         std::uint32_t primId, Vec2f uv,
                                         Vec4f& dest) const
    {
        // TODO: 这里用 simd 增益大吗？
        const auto [i0, i1, i2] = GetIndicesForPrim(primId);
        const auto [n0, n1, n2] = std::array{
            LoadVertexAt(stream, i0),
            LoadVertexAt(stream, i1),
            LoadVertexAt(stream, i2),
        };
        const float u = uv.x;
        const float v = uv.y;
        const Float4 interpolated =
            Add(Add(Scale(n0, 1 - u - v), Scale(n1, u)), Scale(n2, v));
        Store(Normalize(interpolated), glm::value_ptr(dest));
    }

    void TriangleMesh::InterpolateTexCoord(std::uint32_t primId, Vec2f uv,
                                           Vec2f& dest) const
    {
        const std::span<const Vec2f> texCoords = GetTexCoords();
        const auto [i0, i1, i2] = GetIndicesForPrim(primId);
        const float u = uv.x;
        const float v = uv.y;
        dest =
            (texCoords[i0] * (1 - u - v) + texCoords[i1] * u) + texCoords[i2];
    }

    void TriangleMesh::CalcGeometryNormal(std::uint32_t primId,
                                          Vec4f& dest) const
    {
        const auto [i0, i1, i2] = GetIndicesForPrim(primId);
        const std::span<const Vec3f> positions = GetPositions();
        const auto& [p0, p1, p2] =
            std::array{positions[i0], positions[i1], positions[i2]};
        const Vec3f e0 = p1 - p0;
        const Vec3f e1 = p2 - p0;
        dest = Vec4f{glm::normalize(glm::cross(e0, e1)), 0.0f};
    }

    std::array<std::uint16_t, 3>
    TriangleMesh::GetIndicesForPrim(std::uint16_t primId) const
    {
        const std::uint32_t startPos = primId * kTriangleIndicesStride;
        return std::array{m_indices[startPos], m_indices[startPos + 1],
                          m_indices[startPos + 2]};
    }

    Float4 TriangleMesh::LoadVertexAt(std::span<const Vec3f> vertices,
                                      std::uint32_t pos) const
    {
        return FloatsFromMemory(glm::value_ptr(vertices[pos]));
    }
} // namespace rrt
