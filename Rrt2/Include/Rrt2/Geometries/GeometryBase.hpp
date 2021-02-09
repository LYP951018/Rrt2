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
    class MaterialBase;

    class GeometryBase
    {
      public:
        virtual ~GeometryBase();
        virtual void FillPrimitiveArray(AlignedVec<PrimRefStorage>& prims) const = 0;
        virtual std::uint32_t GetPrimitiveCount() const = 0;
        // 使用已有的数据填满 interaction
        virtual void Interpolate(SurfaceInteraction& interaction) const = 0;

        std::uint32_t GetGeomId() const { return m_id; }
        void SetGeomId(std::uint32_t id) { m_id = id; }

        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

        std::shared_ptr<MaterialBase> material;

      private:
        std::uint32_t m_id;
        std::string m_name;
    };
}
