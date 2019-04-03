# Copyright (c) 2019, F.S.. All rights reserved.

cmake_minimum_required(VERSION 3.0.2)

set(FS_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})
string(TIMESTAMP FS_TIME_YEAR "%Y")

# compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFS_DEBUG=1")
endif()

option(FS_CMAKE_MSVC_MD "using dynamic run-time library" ON)
if(WIN32)
  if(FS_CMAKE_MSVC_MD)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
  else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS=1")
elseif(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# configure file
# NOTE! PROJECT_NAME should be UPPERCASE word
function(fs_gen_configure_file PROJECT_NAME)
  if(NOT EXISTS ${CMAKE_SOURCE_DIR}/config.h.in OR FS_NEW_CONFIG_H_IN)
    set(_FS_VERSION_MAJOR_ FS_${PROJECT_NAME}_VERSION_MAJOR)
    set(_FS_VERSION_MINOR_ FS_${PROJECT_NAME}_VERSION_MINOR)
    set(_FS_BUILD_VERSION_ FS_${PROJECT_NAME}_BUILD_VERSION)
    set(_FS_CMAKE_DEFINE_FS_STATIC_LIB_ "#cmakedefine FS_${PROJECT_NAME}_STATIC_LIB")
    set(_FS_STATIC_LIB_ FS_${PROJECT_NAME}_STATIC_LIB)
    set(_FS_SHARED_LIB_ FS_${PROJECT_NAME}_SHARED_LIB)
    set(_FS_CLASS_ FS_${PROJECT_NAME}_CLASS)
    set(_FS_STRUCT_ FS_${PROJECT_NAME}_STRUCT)
    set(_FS_EXPORT_ FS_${PROJECT_NAME}_EXPORT)

    configure_file(${FS_CMAKE_DIR}/config.h.in ${CMAKE_SOURCE_DIR}/config.h.in)
  endif()
endfunction()

# build.sh
configure_file(${FS_CMAKE_DIR}/build.sh ${CMAKE_SOURCE_DIR}/build.sh)

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#set bin suffix
set(FS_BIN_NAME_SUFFIX)
set(FS_BIN_STATIC_SUFFIX "a")
set(FS_PKG_BIN_STATIC_SUFFIX_REGEX ".*\\.a$")
set(FS_BIN_SHARED_SUFFIX "so")
set(FS_PKG_BIN_SHARED_SUFFIX_REGEX ".*\\.so$")

if(CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*")
  set(FS_BIN_NAME_SUFFIX "_gnu")
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
  set(FS_BIN_NAME_SUFFIX "_clang")
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*MSVC.*")
  set(FS_BIN_NAME_SUFFIX "_msvc_${MSVC_VERSION}")
  if(CMAKE_VS_PLATFORM_NAME MATCHES ".*64.*")
    set(FS_BIN_NAME_SUFFIX ${FS_BIN_NAME_SUFFIX}_x64)
  endif()
  if(FS_CMAKE_MSVC_MD)
    set(FS_BIN_NAME_SUFFIX ${FS_BIN_NAME_SUFFIX}_MD)
  else()
    set(FS_BIN_NAME_SUFFIX ${FS_BIN_NAME_SUFFIX}_MT)
  endif()
  set(FS_BIN_STATIC_SUFFIX "lib")
  set(FS_PKG_BIN_STATIC_SUFFIX_REGEX ".*\\.lib$")
  set(FS_BIN_SHARED_SUFFIX "dll")
  set(FS_PKG_BIN_SHARED_SUFFIX_REGEX "\\(.*\\.dll$\\)\\|\\(.*\\.lib$\\)")
else()
  message(FATAL_ERROR "unknown compiler id: " ${CMAKE_CXX_COMPILER_ID})
endif()

# Git HEAD version
set(FS_GIT_UNKNOWN_VERSION "NA")
find_package(Git)
if(GIT_FOUND)
  set(FS_GIT_HEAD_VERSION)
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE FS_GIT_HEAD_VERSION
    ERROR_VARIABLE FS_GIT_ERROR)

  if(FS_GIT_ERROR)
    set(FS_GIT_HEAD_VERSION ${FS_GIT_UNKNOWN_VERSION})
  endif()
  # remove the trailing '\n' of the output variable
  string(REGEX REPLACE "\n$" "" FS_GIT_HEAD_VERSION ${FS_GIT_HEAD_VERSION})
  
  message(STATUS "Git HEAD version: " ${FS_GIT_HEAD_VERSION})
else()
  set(FS_GIT_HEAD_VERSION ${FS_GIT_UNKNOWN_VERSION})
endif()

# search class_name.(h)|(cpp) in class_dir and add to buffer
macro(fs_add_class class_name class_dir buffer)
  
  set(src_file ${CMAKE_CURRENT_SOURCE_DIR}/${class_dir}/${class_name}.cpp)
  if(EXISTS ${src_file})
    set(${buffer} ${${buffer}} ${src_file})
    message(STATUS "src_file: " ${src_file} " added")
  else()
    message(STATUS "src_file: " ${src_file} " not found")
  endif()

  set(header_file ${CMAKE_CURRENT_SOURCE_DIR}/${class_dir}/${class_name}.h)
  if(EXISTS ${header_file})
    set(${buffer} ${${buffer}} ${header_file})
    message(STATUS "header_file: " ${header_file} " added")
  else()
    message(STATUS "header_file: " ${header_file} " not found")
  endif()
endmacro()

set(FS_SYS_INC_DIRS)
set(FS_USER_INC_DIRS)
# include sys directories
macro(fs_add_sys_include_dir target path)
  set(options ABS_PATH)
  cmake_parse_arguments("FS" "${options}" "" "" ${ARGN})
  if(NOT FS_ABS_PATH)
    set(abs_path ${CMAKE_CURRENT_SOURCE_DIR}/${path})
  else()
    set(abs_path ${path})
  endif()
  list(APPEND FS_SYS_INC_DIRS ${abs_path})
  target_include_directories(${target} SYSTEM PRIVATE ${abs_path})
endmacro()

# include user directories
macro(fs_add_user_include_dir target path)
  set(options ABS_PATH)
  cmake_parse_arguments("FS" "${options}" "" "" ${ARGN})
  if(NOT FS_ABS_PATH)
    set(abs_path ${CMAKE_CURRENT_SOURCE_DIR}/${path})
  else()
    set(abs_path ${path})
  endif()
  list(APPEND FS_USER_INC_DIRS ${abs_path})
  target_include_directories(${target} PRIVATE ${abs_path})
endmacro()

# add visual studio $(VC_IncludePath),$(WindowsSDK_IncludePath) to FS_SYS_INC_DIRS
macro(fs_add_vs_default_sys_include_dir abs_path)
  list(APPEND FS_SYS_INC_DIRS ${abs_path})
endmacro()


# fs c++ proj include file(fs_proj.xml)
macro(fs_gen_fs_proj srcs)
  set(FS_SRCS ${${srcs}})
  configure_file(${FS_CMAKE_DIR}/fs_proj.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/fs_proj.xml @ONLY)
  message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/fs_proj.xml generated.")
endmacro()

# @note all dirs and files should be full path
function(fs_gen_fs_pkg_sh pkg_name pkg_header_root_dir pkg_header_regex)
  set(FS_PKG_NAME ${pkg_name})
  set(FS_PKG_HEADER_ROOT_DIR ${pkg_header_root_dir})
  set(FS_PKG_HEADER_REGEX "${pkg_header_regex}")

  if(${ARGC} EQUAL 5)
    set(FS_PKG_LIB_ROOT_DIR ${ARGV3})
    set(FS_PKG_LIB_REGEX "${ARGV4}")
  elseif(${ARGC} EQUAL 7)
    set(FS_PKG_MISC_ROOT_DIR ${ARGV5})
    set(FS_PKG_MISC_REGEX "${ARGV6}")
  elseif(NOT ${ARGC} EQUAL 3)
    message(FATAL_ERROR "wrong number of arguments for funciton: fs_gen_fs_pkg_sh")
  endif()
  
  configure_file(${FS_CMAKE_DIR}/src_pkg.sh.in ${CMAKE_CURRENT_SOURCE_DIR}/fs_pkg.sh @ONLY)
  message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/fs_pkg.sh generated.")
endfunction()
