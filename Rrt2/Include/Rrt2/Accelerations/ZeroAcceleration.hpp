#pragma once

#include "AccelerationBase.hpp"
#include <vector>
#include "PackedTriangle.hpp"

namespace rrt
{
    class Scene;

    class ZeroAcceleration : public AccelerationBase
    {
      public:
        ZeroAcceleration(const Scene* scene);
        std::optional<SurfaceInteraction> Hit(const Ray& ray, float tMin,
                                     float tMax) override;
        void Build() override;

      private:
        const Scene* m_scene;
        std::vector<PackedTriangleStorage> m_packedTriangles;
    };
} // namespace rrt
