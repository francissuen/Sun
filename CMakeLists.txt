cmake_minimum_required(VERSION 3.0)
project(gbUtils)

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=gnu++11)
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

option(gbLUAAPI "export lua api?" OFF)
option(gbUtilsSTATIC "build static or shared lib?" ON)

configure_file(${PROJECT_SOURCE_DIR}/gbUtilsConfig.h.in ${PROJECT_SOURCE_DIR}/gbUtilsConfig.h)

set(LIB_NAME "gbUtils")

set(gb_src_files)
set(gb_header_files)

macro(gb_add_class clsNames dir)
  foreach(name ${clsNames})
    set(src_file ${dir}/${name}.cpp)
    set(header_file ${dir}/${name}.h)

    # SRC
    if(EXISTS ${PROJECT_SOURCE_DIR}/${src_file})
      message("file@" ${src_file} " added")
      set(gb_src_files ${gb_src_files} ${src_file})
    else()
      message(WARNING "file@" ${src_file} " not exist")
    endif()

    #HEADER
    if(EXISTS ${PROJECT_SOURCE_DIR}/${header_file})
      message("file@" ${header_file} " added")
      set(gb_header_files ${gb_header_files} ${header_file})
    else()
      message(WARNING "file@" ${header_file} " not exist")
    endif()
  endforeach()
endmacro()

gb_add_class(gbLog Log)
gb_add_class(gbTime Time)
gb_add_class(gbString String)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(gbUtilsSTATIC)
  add_library(${LIB_NAME} STATIC
    ${gb_src_files} ${gb_header_files})
else()
  add_definitions(-DgbUtilsEXPORT)
  add_library(${LIB_NAME} SHARED
    ${gb_src_files} ${gb_header_files})
endif()

set_target_properties(${LIB_NAME}
  PROPERTIES
  DEBUG_OUTPUT_NAME ${LIB_NAME}d
  RELEASE_OUTPUT_NAME ${LIB_NAME}
  )

# INSTALL
if(UNIX)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/gbUtils)
endif()

message("install dir:" " ${CMAKE_INSTALL_PREFIX}")

install(TARGETS ${LIB_NAME} ARCHIVE DESTINATION lib)
# install(FILES ${gb_header_files} DESTINATION include)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION include
  FILES_MATCHING PATTERN "*.h"
  PATTERN "build" EXCLUDE
  PATTERN ".*" EXCLUDE
  )
