cmake_minimum_required(VERSION 3.0)

project(gbtest)
include(../gbCMake/gbCommon.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../gbCMake/)

add_executable(gbtest
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

target_link_libraries(gbtest
  gbUtils
  optimized ${lua_LIBRARIES}
  debug ${lua_LIBRARIES_d}
  ${pthread_lib}
  ${dl_lib}
  )

set_target_properties(gbtest
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}")
