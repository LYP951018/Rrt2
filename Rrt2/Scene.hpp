#pragma once

#include <pcg_variants.h>
#include <gsl/span>
#include "Accelerations/AccelerationBase.hpp"
#include <vector>

class TriangleMesh;

class Scene
{
  public:
    Scene();
    // std::optional<HitRecord> Trace(const Ray& ray);
    const GeometryBase* GetGeometryAt(std::uint32_t i) const;
    const std::vector<std::unique_ptr<GeometryBase>>& GetGeometries() const { return m_geometries; }
    void AddTriangleMesh(std::unique_ptr<TriangleMesh> triangleMesh);
    std::uint32_t GetAvailableGeomId() const;
    void Ready();

  private:
    mutable pcg32_random_t m_randState;
    std::unique_ptr<AccelerationBase> m_acceleration;
    std::vector<std::unique_ptr<GeometryBase>> m_geometries;
};