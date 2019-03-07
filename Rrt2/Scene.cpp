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

const GeometryBase* Scene::GetGeometryAt(std::uint32_t i) const { return m_geometries[i].get(); }

void Scene::AddTriangleMesh(std::unique_ptr<TriangleMesh> triangleMesh)
{
    triangleMesh->SetGeomId(GetAvailableGeomId());
    m_geometries.push_back(std::move(triangleMesh));
}

std::uint32_t Scene::GetAvailableGeomId() const
{
    return static_cast<std::uint32_t>(m_geometries.size());
}

void Scene::Ready() { m_acceleration->Build(); }
