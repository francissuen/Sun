cmake_minimum_required(VERSION 3.0)
project(sun)

include(cmake_utility/CMakeLists.txt)

set(FS_SUN_BUILD_VERSION ${FS_GIT_HEAD_VERSION})
set(FS_SUN_VERSION_MAJOR 0)
set(FS_SUN_VERSION_MINOR 0)

option(FS_SUN_STATIC_LIB "build static lib?" ON)
# option(FS_SUN_VERBOSE "build lib verbosely" OFF)

# if(FS_SUN_VERBOSE)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFS_SUN_VERBOSE")
# endif()

configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_SOURCE_DIR}/src/config.h)

set(srcs)
fs_add_class(ns src srcs)
fs_add_class(logger src srcs)
fs_add_class(time src srcs)
fs_add_class(string src srcs)
fs_add_class(filesystem src srcs)
fs_add_class(event src srcs)
fs_add_class(async src srcs)
fs_add_class(json src srcs)
fs_add_class(range_viewer src srcs)
fs_add_class(singlteton src srcs)
fs_add_class(utility src srcs)
fs_add_class(debug src srcs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(bin_suffix)
if(FS_SUN_STATIC_LIB)
  add_library(sun
    ${srcs})
  set(bin_suffix ${FS_BIN_STATIC_SUFFIX})
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFS_SUN_SHARED_LIB=1")
  add_library(sun SHARED
    ${srcs})
  set(bin_suffix ${FS_BIN_SHARED_SUFFIX})
  target_compile_definitions(sun PRIVATE FS_SUN_SHARED_LIB)
endif()

set(lib_name "sun${FS_BIN_NAME_SUFFIX}")

set_target_properties(sun
  PROPERTIES
  OUTPUT_NAME_DEBUG ${lib_name}d
  OUTPUT_NAME_RELEASE ${lib_name}
  )

# get_target_property(get_name sun OUTPUT_NAME_DEBUG)

# set(CMAKE_INSTALL_PREFIX "C:/Program\ Files/sun")


# install(TARGETS sun ARCHIVE DESTINATION .)

# set(headers)
# foreach(src IN LISTS srcs)
#   if(${src} MATCHES ".*\\.h") 
#     set(headers ${headers} ${src})
#     message("header: " ${src})
#   endif()
# endforeach()
# set(headers ${headers} ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h)

# install(FILES ${headers} DESTINATION .)

fs_gen_fs_proj(srcs)

# set(pkg_libs
#   ${CMAKE_CURRENT_SOURCE_DIR}/bin/sun.lib
#   ${CMAKE_CURRENT_SOURCE_DIR}/bin/sund.lib)

set(bin_regex)
if(FS_SUN_STATIC_LIB)
  set(bin_regex ${FS_PKG_BIN_STATIC_SUFFIX_REGEX})
else()
  set(bin_regex ${FS_PKG_BIN_SHARED_SUFFIX_REGEX})
endif()

fs_gen_fs_pkg_py(sun
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/bin "${bin_regex}")

add_subdirectory(test)
