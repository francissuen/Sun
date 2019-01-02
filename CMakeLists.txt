cmake_minimum_required(VERSION 3.0)
project(fsCommon)

include(fsCMake/fsCommon.cmake)

set(FS_COMMON_BUILD_VERSION ${FS_GIT_HEAD_VERSION})
set(FS_COMMON_VERSION_MAJOR 0)
set(FS_COMMON_VERSION_MINOR 0)

option(FS_COMMON_LUA_API "bind lua api?" OFF)
option(FS_COMMON_STATIC_LIB "build static lib?" ON)
option(FS_COMMON_MULTI_THREADS "support multi-threads" ON)

configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_SOURCE_DIR}/src/config.h)

set(srcs)
fs_add_class(ns src srcs)
fs_add_class(common src srcs)
fs_add_class(logger src srcs)
fs_add_class(time src srcs)
fs_add_class(string src srcs)
# fs_add_class(vector src srcs)
fs_add_class(concurrency src srcs)
fs_add_class(filesystem src srcs)
fs_add_class(callback src srcs)

if(FS_COMMON_LUA_API)
  fs_add_class(luastate src srcs)
  fs_add_class(luatable src srcs)
endif()

# find_package(lua REQUIRED)
# include_directories(SYSTEM ${lua_INCLUDE_DIRS})

# fs_add_class(luacpp_type_conversion src srcs)
# fs_add_class(luacpp src srcs)
fs_add_class(args src srcs)
fs_add_class(file src srcs)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(FS_COMMON_STATIC_LIB)
  add_library(fsCommon
    ${srcs}
    )
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFS_COMMON_SHARED_LIB=1")
  add_library(fsCommon SHARED
    ${srcs}
    )
endif()

set_target_properties(fsCommon
  PROPERTIES
  DEBUG_OUTPUT_NAME fsCommond
  RELEASE_OUTPUT_NAME fsCommon
  )

set(CMAKE_INSTALL_PREFIX "C:/Program\ Files/fsCommon")


install(TARGETS fsCommon ARCHIVE DESTINATION .)

set(headers)
foreach(src IN LISTS srcs)
  if(${src} MATCHES ".*\\.h") 
    set(headers ${headers} ${src})
    message("header: " ${src})
  endif()
endforeach()
set(headers ${headers} src/config.h)

install(FILES ${headers} DESTINATION .)
