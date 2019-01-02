cmake_minimum_required(VERSION 3.0)

project(fstest)
include(../fsCMake/fsCommon.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../fsCMake/)

add_executable(fstest
  test.cpp
  )

set(pthread_lib)
set(dl_lib)
if(UNIX)
  set(pthread_lib -lpthread)
  set(dl_lib -ldl)
endif()

find_package(lua REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_SOURCE_DIR}/../build)

target_link_libraries(fstest
  fsCommon
  # optimized ${lua_LIBRARIES}
  # debug ${lua_LIBRARIES_d}
  ${pthread_lib}
  ${dl_lib}
  )

set_target_properties(fstest
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}")
