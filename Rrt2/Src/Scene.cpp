#include "Scene.hpp"
#include "Geometries/TriangleMesh.hpp"
#include "Accelerations/Bvh.hpp"
#include <cmath>
#include <ctime>

Scene::Scene()
{
    pcg32_srandom_r(&m_randState, std::time(0), (intptr_t)&m_randState);
    m_acceleration = std::make_unique<Bvh>(this);
}

std::optional<HitRecord> Scene::Trace(const Ray& ray, float tMin, float tMax)
{
    return m_acceleration->Hit(ray, tMin, tMax);
}

const GeometryBase* Scene::GetGeometryAt(std::uint32_t i) const { return m_geometries[i].get(); }

void Scene::AddGeometry(std::unique_ptr<GeometryBase> geometry)
{
    geometry->SetGeomId(GetAvailableGeomId());
    m_geometries.push_back(std::move(geometry));
}

std::uint32_t Scene::GetAvailableGeomId() const
{
    return static_cast<std::uint32_t>(m_geometries.size());
}

void Scene::Ready() { m_acceleration->Build(); }
