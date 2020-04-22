cmake_minimum_required(VERSION 3.15.0)


add_library(Rrt2 STATIC)

file(GLOB_RECURSE RRT2_SRCS ./Src/*.cpp)
file(GLOB_RECURSE RRT2_HAEDERS ./Include/*.hpp)
set(NATVIS_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Natvis/Rrt2.natvis)

target_sources(Rrt2 PRIVATE ${RRT2_SRCS} ${RRT2_HAEDERS} ${NATVIS_FILE_PATH})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Src FILES ${RRT2_SRCS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Include FILES ${RRT2_EXPORT_HAEDERS})

target_include_directories(Rrt2 PUBLIC Include)
add_subdirectory(3rd/pcg-c)
add_subdirectory(3rd/glm)
add_subdirectory(3rd/GSL)
set(BUILD_ASSIMP_TOOLS OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static assimp")
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "Disable all importers by default")
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "Disable all exporters by default")
SET(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "Enable the obj importer")
add_subdirectory(3rd/assimp)
add_subdirectory(3rd/fmt)
#target_compile_features(Rrt2 PUBLIC cxx_std_20)
target_link_libraries(Rrt2 PUBLIC pcg-c glm::glm GSL assimp::assimp fmt::fmt)

if (MSVC)
    target_compile_options(Rrt2 PUBLIC /utf-8 /permissive- /std:c++latest /arch:AVX2)
else()
    target_compile_options(Rrt2 PUBLIC -std=c++2a -mavx)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(Rrt2 PUBLIC -stdlib=libc++)
    endif()
endif()
