#pragma once

#include <memory_resource>
#include <vector>

template<typename T>
using AlignedVec = std::pmr::vector<T>;