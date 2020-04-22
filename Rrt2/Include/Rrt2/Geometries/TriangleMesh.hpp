#pragma once

#include "GeometryBase.hpp"
#include "../BoundingBox.hpp"
#include "../SharedBuffer.hpp"
#include <vector>
#include <span>
#include <memory>

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
                     std::vector<std::uint16_t> indices,
                     const Vec3f* positions, const Vec3f* normals,
                     const Vec3f* tangents);
        std::span<const Vec3f> GetNormals() const;
        std::span<const Vec3f> GetPositions() const;
        std::uint32_t GetPrimitiveCount() const override
        {
            return static_cast<std::uint32_t>(m_indices.size());
        }
        void
        FillPrimitiveArray(AlignedVec<PrimRefStorage>& prims) const override;

      private:
        std::uint32_t m_verticesCount;
        std::unique_ptr<Vec3f[]> m_positions;
        std::unique_ptr<Vec3f[]> m_normals;
        std::unique_ptr<Vec3f[]> m_tangents;
        std::vector<std::uint16_t> m_indices;

        void CopyVertices(const std::unique_ptr<Vec3f[]>& dest, const Vec3f* src);
        void CalculateTangentVecs();
    };
} // namespace rrt
