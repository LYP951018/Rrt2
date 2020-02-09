#pragma once

#include <optional>
#include "../HitRecord.hpp"
#include "../AlignedVec.hpp"

namespace rrt
{
    class Ray;
    class BoundingBox;
    class PrimRefStorage;

    class GeometryBase
    {
      public:
        // virtual std::optional<HitRecord> Hit(const Ray& ray, float tMin, float tMax) = 0;
        // virtual const BoundingBox& GetBoundingBox() const = 0;
        virtual ~GeometryBase();
        virtual void FillPrimitiveArray(AlignedVec<PrimRefStorage>& prims) const = 0;
        virtual std::uint32_t GetPrimitiveCount() const = 0;
        std::uint32_t GetGeomId() const { return m_id; }
        void SetGeomId(std::uint32_t id) { m_id = id; }

      private:
        std::uint32_t m_id;
    };
}
