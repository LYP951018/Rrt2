cmake_minimum_required(VERSION 3.15.0)


add_library(Rrt2 STATIC)

aux_source_directory(./Src RRT2_SRCS)
target_sources(Rrt2 PRIVATE ${RRT2_SRCS})
target_include_directories(Rrt2 PRIVATE Src PUBLIC Export)
add_subdirectory(3rd/pcg-c)
target_compile_features(Rrt2 PUBLIC cxx_std_20)
target_link_libraries(Rrt2 PUBLIC pcg-c)

if (MSVC)
    target_compile_options(Rrt2 PRIVATE /utf-8 /permissive-)
endif()
