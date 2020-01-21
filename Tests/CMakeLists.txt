cmake_minimum_required(VERSION 3.15.0)
enable_testing()
add_test(NAME RRT2_TESTS COMMAND *)
add_executable(Rrt2Tests)
aux_source_directory(. RRT2_TEST_SRCS)
target_sources(Rrt2Tests PRIVATE ${RRT2_TEST_SRCS})
# HACK HACK
target_include_directories(Rrt2Tests PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Rrt2/Src>)
target_link_libraries(Rrt2Tests PRIVATE Rrt2)
