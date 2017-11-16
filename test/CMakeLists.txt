cmake_minimum_required(VERSION 3.0)

project(gbtest)
include(../gbCMake/gbCommon.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../gbCMake/)

find_package(gbUtils REQUIRED)

add_executable(gbtest
  test.cpp
  )


target_link_libraries(gbtest
  debug ${gbUtils_LIBRARIES_d}
  optimized ${gbUtils_LIBRARIES}
  )
