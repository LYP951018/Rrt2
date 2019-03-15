#pragma once

#include <memory>
#include "AlignedVec.hpp"

// TODO: 引用计数、数组大小和数组内容可以共用内存，以后再重新实现一个
template<typename T>
using SharedBuffer = std::shared_ptr<std::vector<T>>;