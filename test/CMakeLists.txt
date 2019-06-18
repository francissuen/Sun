cmake_minimum_required(VERSION 3.0)

project(sun-test)
include(../fsCMake/CMakeLists.txt)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../fsCMake/)

add_executable(sun-test
  src/main.cpp
  )

set(pthread_lib)
set(dl_lib)
if(UNIX)
  set(pthread_lib -lpthread)
  set(dl_lib -ldl)
endif()

target_link_libraries(sun-test
  sun
  # optimized ${lua_LIBRARIES}
  # debug ${lua_LIBRARIES_d}
  ${pthread_lib}
  ${dl_lib}
  )

set_target_properties(sun-test
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")

add_dependencies(sun-test sun)
