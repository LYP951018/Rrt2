﻿#include "Rrt2/Scene.hpp"
#include "Rrt2/Geometries/TriangleMesh.hpp"
#include "Rrt2/Accelerations/Bvh.hpp"
#include <cmath>
#include <ctime>

namespace rrt
{
    Scene::Scene()
    {
        pcg32_srandom_r(&m_randState, std::time(0), (intptr_t)&m_randState);
        m_acceleration = std::make_unique<Bvh>(this);
    }

    std::optional<SurfaceInteraction> Scene::Trace(const Ray& ray)
    {
        return m_acceleration->Hit(ray);
    }

    const GeometryBase* Scene::GetGeometryAt(std::uint32_t i) const
    {
        return m_geometries[i].get();
    }

    void Scene::AddTriangleMesh(std::unique_ptr<TriangleMesh> geometry)
    {
        geometry->SetGeomId(GetAvailableGeomId());
        m_geometries.push_back(std::move(geometry));
    }

    std::uint32_t Scene::GetAvailableGeomId() const
    {
        return static_cast<std::uint32_t>(m_geometries.size());
    }

    void Scene::Ready() { m_acceleration->Build(); }
} // namespace rrt
