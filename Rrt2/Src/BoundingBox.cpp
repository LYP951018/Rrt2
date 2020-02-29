#include "BoundingBox.hpp"
#include "StorageLoadStore.hpp"

namespace rrt
{
    void VECTORCALL BoundingBox::Extend(BoundingBox bbox)
    {
        corners[0] = Min(corners[0], bbox.corners[0]);
        corners[1] = Max(corners[1], bbox.corners[1]);
    }

    void VECTORCALL BoundingBox::Extend(Float4 point)
    {
        corners[0] = Min(corners[0], point);
        corners[1] = Max(corners[1], point);
    }

    // (x, y, z) * (z, x, y)
    float BoundingBox::GetHalfArea() const
    {
        const Float4 size = GetSize();
        return SumUpLanes(Mul(size, PermuteFloats<2, 0, 1, 3>(size)));
    }

    void BoundingBox::StoreTo(BoundingBoxStorage& storage) const
    {
        StoreStorage(corners[0], storage.corners[0]);
        StoreStorage(corners[1], storage.corners[1]);
    }

    BoundingBox BoundingBoxStorage::Load() const
    {
        return BoundingBox{
            .corners = {LoadStorage(corners[0]), LoadStorage(corners[1])}};
    }
}
