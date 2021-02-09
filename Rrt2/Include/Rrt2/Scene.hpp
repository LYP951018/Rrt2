#pragma once

#include <memory>
#include "Accelerations/AccelerationBase.hpp"
#include <vector>
#include <optional>
#include <span>
#include "HitRecord.hpp"
#include "Ray.hpp"
#include "Lights/LightBase.hpp"

namespace rrt
{
    class TriangleMesh;

    class Scene
    {
      public:
        Scene();
        std::optional<SurfaceInteraction> Trace(const Ray& ray) const;
        bool Hit(const Ray& ray) const;
        const GeometryBase* GetGeometryAt(std::uint32_t i) const;
        const std::vector<std::unique_ptr<GeometryBase>>& GetGeometries() const
        {
            return m_geometries;
        }
        void AddTriangleMesh(std::unique_ptr<TriangleMesh> geometry);
        std::uint32_t GetAvailableGeomId() const;
        void Ready();
        template<typename TGeometry>
        const TGeometry* GetGeometryAs(std::uint32_t geomId) const
        {
            const GeometryBase* geometry = m_geometries[geomId].get();
            assert(dynamic_cast<const TGeometry*>(geometry) != nullptr);
            return (const TGeometry*)geometry;
        }

        std::span<const std::unique_ptr<LightBase>> GetLights() const {
            return std::span{ m_lights };
        }

      private:
        std::unique_ptr<AccelerationBase> m_acceleration;
        std::vector<std::unique_ptr<GeometryBase>> m_geometries;
        std::vector<std::unique_ptr<LightBase>> m_lights;

        void Interpolate(SurfaceInteraction& interaction) const;
    };
} // namespace rrt
