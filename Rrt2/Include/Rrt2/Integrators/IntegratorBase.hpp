#pragma once


namespace rrt
{
    class Scene;

    class IntegratorBase
    {
    public:
        virtual void Render(const Scene& scene);
        virtual ~IntegratorBase();
    };
}