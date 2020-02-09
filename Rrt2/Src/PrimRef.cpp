#include "Rrt2/PrimRef.hpp"
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Rrt2/StdExtras.hpp"

namespace rrt
{
    PrimRefStorage::PrimRefStorage(const BoundingBox& bounds,
                                   std::uint32_t geomId, std::uint32_t primId)
    {
        Store(SelectCombine<0, 0, 0, 1>(bounds.corners[0],
                                        AsFloats(MakeInts(geomId))),
              glm::value_ptr(lower));
        Store(SelectCombine<0, 0, 0, 1>(bounds.corners[1],
                                        AsFloats(MakeInts(primId))),
              glm::value_ptr(upper));
    }

    Float4 LoadedPrimRef::GetCenter() const
    {
        return Div(Add(lower, upper), MakeFloats(2.0f));
    }

    // std::uint32_t PrimRefStorage::GetPrimId() const { return
    // bit_cast<std::uint32_t>(Fourth(upper)); }

    // std::uint32_t PrimRefStorage::GetGeomId() const { return
    // bit_cast<std::uint32_t>(Fourth(lower)); }
} // namespace rrt
