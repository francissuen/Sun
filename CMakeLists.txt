# Copyright (C) 2020 Francis Sun, all rights reserved.

cmake_minimum_required(VERSION 3.0)

set(SUN "sun")
project(${SUN} VERSION 0.1.0 LANGUAGES CXX)

option(FS_SUN_STATIC_LIB "Build a static lib?" ON)

set(FS_VESTA_USE_MULTI_THREADS ON)
include(vesta/CMakeLists.txt)

# generate the config.h
fs_vesta_generate_config_h_in(FS_SUN)
set(FS_SUN_VERSION ${PROJECT_VERSION})
set(FS_SUN_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(FS_SUN_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(FS_SUN_BUILD_VERSION ${FS_VESTA_GIT_HEAD_VERSION})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h)

if(FS_SUN_STATIC_LIB)
  add_library(${SUN} "")
else()
  add_library(${SUN} SHARED "")
  target_compile_definitions(${SUN} PRIVATE FS_SUN_SHARED_LIB)
endif()

add_subdirectory(src)
add_subdirectory(test)

fs_vesta_generate_vesta_pkg(${SUN} ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/bin ${FS_SUN_STATIC_LIB})
