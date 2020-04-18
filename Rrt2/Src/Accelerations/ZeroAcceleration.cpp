#include "Rrt2/Accelerations/ZeroAcceleration.hpp"
#include "Rrt2/Accelerations/Bvh.hpp"
#include "Rrt2/Scene.hpp"
#include "Rrt2/Geometries/GeometryBase.hpp"
#include "Rrt2/Geometries/TriangleMesh.hpp"
#include "Rrt2/Accelerations/PackedRay.hpp"

namespace rrt
{
    ZeroAcceleration::ZeroAcceleration(const Scene* scene) : m_scene{scene} {}

    std::optional<SurfaceInteraction> ZeroAcceleration::Hit(const Ray& ray, float tMin,
                                                   float tMax)
    {
        const PackedRay packedRay{ray};
        std::optional<SurfaceInteraction> hitRecord;
        for (const PackedTriangleStorage& triangleStorage : m_packedTriangles)
        {
            const PackedTriangle packedTriangle = triangleStorage.Load();
            const std::optional<SurfaceInteraction> childRecord =
                packedTriangle.Hit(packedRay, ray, tMin, tMax);
            if (!childRecord)
                continue;
            if (!hitRecord || hitRecord.value().t > childRecord.value().t)
                hitRecord = childRecord;
        }
        return hitRecord;
    }

    void ZeroAcceleration::Build()
    {
        const auto& geometries = m_scene->GetGeometries();
        for (const std::unique_ptr<GeometryBase>& geometry : geometries)
        {
            TriangleMesh* mesh = dynamic_cast<TriangleMesh*>(geometry.get());
            for (std::uint32_t i = 0; i < mesh->GetPrimitiveCount(); ++i)
            {
                SingleTriangle triangle = mesh->GetPrimitiveAt(i);
                PackedTriangleStorage packed;
                packed.Set(0, triangle, i, mesh->GetGeomId());
                m_packedTriangles.push_back(packed);
            }
        }
    }
} // namespace rrt
