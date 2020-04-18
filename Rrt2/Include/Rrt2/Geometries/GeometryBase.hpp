#pragma once

#include <optional>
#include <string>
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
        // virtual std::optional<SurfaceInteraction> Hit(const Ray& ray, float tMin, float tMax) = 0;
        // virtual const BoundingBox& GetBoundingBox() const = 0;
        virtual ~GeometryBase();
        virtual void FillPrimitiveArray(AlignedVec<PrimRefStorage>& prims) const = 0;
        virtual std::uint32_t GetPrimitiveCount() const = 0;

        std::uint32_t GetGeomId() const { return m_id; }
        void SetGeomId(std::uint32_t id) { m_id = id; }

        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

      private:
        std::uint32_t m_id;
        std::string m_name;
    };
}
