cmake_minimum_required(VERSION 3.0)

project(gbtest)
include(../gbCMake/gbCommon.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../gbCMake/)

find_package(gbUtils REQUIRED)
# include_directories(${gbUtils_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(gbtest
  test.cpp
  )


target_link_libraries(gbtest
  ${gbUtils_LIBRARIES}
  -lpthread
  )
