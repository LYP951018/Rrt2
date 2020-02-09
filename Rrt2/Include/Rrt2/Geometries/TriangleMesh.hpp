#pragma once

#include "GeometryBase.hpp"
#include "../BoundingBox.hpp"
#include "../SharedBuffer.hpp"
#include <vector>
#include <gsl/span>

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
        std::uint16_t x, y, z;
    };

    class TriangleMesh : public GeometryBase
    {
      public:
        static inline constexpr std::size_t kTriangleIndicesStride = 3;

        TriangleMesh(std::vector<Vec3f> positions,
                     std::vector<TriangleIndices> indices);
        SingleTriangle GetPrimitiveAt(std::uint32_t id) const;
        std::uint32_t GetPrimitiveCount() const override
        {
            return static_cast<std::uint32_t>(m_indices.size());
        }
        void
        FillPrimitiveArray(AlignedVec<PrimRefStorage>& prims) const override;

      private:
        // FIXME: SharedView / span here.
        std::vector<Vec3f> m_positions;
        std::vector<TriangleIndices> m_indices;
    };
} // namespace rrt
