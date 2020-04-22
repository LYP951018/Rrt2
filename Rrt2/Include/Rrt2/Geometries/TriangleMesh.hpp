#pragma once

#include "GeometryBase.hpp"
#include "../BoundingBox.hpp"
#include "../SharedBuffer.hpp"
#include <vector>
#include <gsl/span>
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

    struct TriangleIndices
    {
        std::uint16_t v0, v1, v2;
    };

    class TriangleMesh : public GeometryBase
    {
      public:
        static inline constexpr std::size_t kTriangleIndicesStride = 3;

        TriangleMesh(std::uint32_t verticesCount,
                     std::vector<TriangleIndices> indices,
                     const Vec3f* positions, const Vec3f* normals,
                     const Vec3f* tangents);
        SingleTriangle GetPrimitiveAt(std::uint32_t id) const;
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
        std::vector<TriangleIndices> m_indices;

        void CopyVertices(const std::unique_ptr<Vec3f[]>& dest, const Vec3f* src);
        void CalculateTangentVecs();
    };
} // namespace rrt
