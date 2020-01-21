#include "Vec3.hpp"
#include <ostream>

#define DEF_VEC(type)               \
    template class type<Floats<4>>; \
    template class type<Floats<8>>; \
    template class type<float>;

DEF_VEC(Vec2T)

DEF_VEC(Vec3T)
