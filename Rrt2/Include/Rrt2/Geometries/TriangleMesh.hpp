#pragma once

#include "GeometryBase.hpp"
#include "../BoundingBox.hpp"
#include "../SharedBuffer.hpp"
#include "../Simd.hpp"
#include <vector>
#include <span>
#include <memory>
#include <cstdint>
#include <array>

namespace rrt
{
    class Material;

    struct SingleTriangle
    {
        Vec3f v0;
        Vec3f v1;
        Vec3f v2;
    };

    class TriangleMesh : public GeometryBase
    {
      public:
        static inline constexpr std::size_t kTriangleIndicesStride = 3;

        TriangleMesh(std::uint32_t verticesCount,
                     std::vector<std::uint16_t> indices, const Vec3f* positions,
                     const Vec2f* texCoords, const Vec3f* normals,
                     const Vec3f* tangents);
        std::span<const Vec3f> GetNormals() const;
        std::span<const Vec3f> GetPositions() const;
        std::span<const Vec3f> GetTangents() const;
        std::span<const Vec2f> GetTexCoords() const;
        std::uint32_t GetPrimitiveCount() const override
        {
            return static_cast<std::uint32_t>(m_indices.size());
        }
        void
        FillPrimitiveArray(AlignedVec<PrimRefStorage>& prims) const override;
        void Interpolate(SurfaceInteraction& interaction) const override;

      private:
        std::uint32_t m_verticesCount;
        std::unique_ptr<Vec3f[]> m_positions;
        std::unique_ptr<Vec2f[]> m_texCoords;
        std::unique_ptr<Vec3f[]> m_normals;
        std::unique_ptr<Vec3f[]> m_tangents;
        std::vector<std::uint16_t> m_indices;

        void CopyVertices(const std::unique_ptr<Vec3f[]>& dest,
                          const Vec3f* src);
        void CalculateTangentVecs();
        void InteroplateDpDu();
        void InterpolateStream(std::span<const Vec3f> stream,
                               std::uint32_t primId, Vec2f uv,
                               Vec4f& dest) const;
        void InterpolateTexCoord(std::uint32_t primId, Vec2f uv, Vec2f& dest) const;
        void CalcGeometryNormal(std::uint32_t primId, Vec4f& dest) const;
        std::array<std::uint16_t, 3>
        GetIndicesForPrim(std::uint16_t primId) const;
        Float4 LoadVertexAt(std::span<const Vec3f> vertices,
                            std::uint32_t pos) const;
    };
} // namespace rrt
