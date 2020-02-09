#pragma once

#include <vector>
#include <cstddef>
#include <gsl/span>

namespace rrt
{
    struct StreamInfo
    {
        std::uint32_t stride;
    };

    class Mesh
    {
      public:
        template<typename T>
        gsl::span<T> GetStreamAs(std::size_t index) const
        {
            const std::vector<std::byte>& channel = m_channels[index];
            return gsl::make_span(reinterpret_cast<const T*>(channel.data()),
                                  channel.size() / sizeof(T));
        }

        gsl::span<const Vec3f> GetPositions() const { return GetStreamAs<const Vec3f>(0); }

      private:
        std::vector<std::vector<std::byte>> m_channels;
        std::vector<StreamInfo> m_streamInfos;
        std::vector<std::uint16_t> m_indices;
    };
}
