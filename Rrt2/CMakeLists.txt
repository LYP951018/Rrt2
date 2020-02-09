cmake_minimum_required(VERSION 3.15.0)


add_library(Rrt2 STATIC)

file(GLOB_RECURSE RRT2_SRCS
     ./Src/*.hpp ./Src/*.cpp)

file(GLOB_RECURSE RRT2_EXPORT_HAEDERS
     ./Export/*.hpp)

target_sources(Rrt2 PRIVATE ${RRT2_SRCS} ${RRT2_EXPORT_HAEDERS})
target_include_directories(Rrt2 PRIVATE Src INTERFACE Export PRIVATE Export/Rrt2)
add_subdirectory(3rd/pcg-c)
add_subdirectory(3rd/glm)
add_subdirectory(3rd/GSL)
#target_compile_features(Rrt2 PUBLIC cxx_std_20)
target_link_libraries(Rrt2 PUBLIC pcg-c glm::glm GSL)

if (MSVC)
    target_compile_options(Rrt2 PUBLIC /utf-8 /permissive- /std:c++latest /arch:AVX2)
else()
    target_compile_options(Rrt2 PUBLIC -std=c++2a -mavx)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(Rrt2 PUBLIC -stdlib=libstdc++)
    endif()
endif()
