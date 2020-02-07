cmake_minimum_required(VERSION 3.15.0)


add_library(Rrt2 STATIC)

aux_source_directory(./Src RRT2_SRCS)
aux_source_directory(./Src/Accelerations RRT2_ACC_SRCS)
aux_source_directory(./Src/Geometries RRT2_GEO_SRCS)

target_sources(Rrt2 PRIVATE ${RRT2_SRCS} ${RRT2_ACC_SRCS} ${RRT2_GEO_SRCS})
target_include_directories(Rrt2 PRIVATE Src INTERFACE Export PRIVATE Export/Rrt2)
add_subdirectory(3rd/pcg-c)
add_subdirectory(3rd/glm)
add_subdirectory(3rd/GSL)
#target_compile_features(Rrt2 PUBLIC cxx_std_20)
target_link_libraries(Rrt2 PUBLIC pcg-c glm::glm GSL)

if (MSVC)
    target_compile_options(Rrt2 PUBLIC /utf-8 /permissive-)
else()
    target_compile_options(Rrt2 PUBLIC -std=c++2a -mavx)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(Rrt2 PUBLIC -stdlib=libstdc++)
    endif()
endif()
