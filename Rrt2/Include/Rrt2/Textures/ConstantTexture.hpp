#pragma once

#include "../TextureBase.hpp"

namespace rrt
{
    template<typename PixelT>
    class ConstantTexture : public TextureBase<PixelT>
    {
      public:
        ConstantTexture(const PixelT& pixel) : m_constantPix{pixel} {}

        PixelT Eval(const SurfaceInteraction&) override
        {
            return m_constantPix;
        }

      private:
        PixelT m_constantPix;
    };
} // namespace rrt