#pragma once

#include "HitRecord.hpp"

namespace rrt
{
    template<typename PixelT>
    class TextureBase
    {
      public:
        virtual PixelT Eval(const SurfaceInteraction& interaction) const = 0;
        virtual ~TextureBase() = default;
    };
} // namespace rrt
