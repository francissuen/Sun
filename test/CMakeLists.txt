cmake_minimum_required(VERSION 3.0)

project(test_sun)
include(../cmake_utility/CMakeLists.txt)
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../fsCMake/)

add_executable(test_sun
  src/main.cpp
  )

target_link_libraries(test_sun
  sun
  )

set_target_properties(test_sun
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")

add_dependencies(test_sun sun)
