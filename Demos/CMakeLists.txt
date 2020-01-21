cmake_minimum_required(VERSION 3.15.0)


MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

FOREACH(RRT2_DEMO_NAME ${SUBDIRS})
message(${RRT2_DEMO_NAME})
  add_executable(${RRT2_DEMO_NAME})
  aux_source_directory(${RRT2_DEMO_NAME} RRT2_DEMO_SRCS)
  target_sources(${RRT2_DEMO_NAME} PRIVATE ${RRT2_DEMO_SRCS})
  target_link_libraries(${RRT2_DEMO_NAME} PRIVATE Rrt2)
ENDFOREACH()