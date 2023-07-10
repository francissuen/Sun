# Copyright (C) 2020 Francis Sun, all rights reserved.

cmake_minimum_required(VERSION 3.0)

set(SUN "sun")
project(${SUN} VERSION 0.1.0 LANGUAGES CXX)

include(vesta/CMakeLists.txt)

# generate the config.h
fs_vesta_generate_config_h(FS_SUN ${SUN})

option(FS_SUN_STATIC_LIB "Build a static lib?" ON)

if(FS_SUN_STATIC_LIB)
  add_library(${SUN} "")
  target_compile_definitions(${SUN} PUBLIC FS_SUN_STATIC_LIB)
else()
  add_library(${SUN} SHARED "")
  target_compile_definitions(${SUN} PRIVATE FS_SUN_SHARED_LIB)
endif()

add_subdirectory(src)
add_subdirectory(test)

fs_vesta_generate_vesta_pkg(${SUN} ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/bin ${FS_SUN_STATIC_LIB})
