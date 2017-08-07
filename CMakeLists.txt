cmake_minimum_required(VERSION 3.6)
project(gbUtils)

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=gnu++11)
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

option(gbLUAAPI "export lua api?" OFF)
option(gbUtilsSTATIC "build static or shared lib?" ON)

configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_SOURCE_DIR}/config.h)

set(LIB_NAME "gbUtils")

set(gb_src_files)
set(gb_header_files)

macro(gb_src_add_class dir clsNames)
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

gb_src_add_class(Log gbLog)
gb_src_add_class(Time gbTime)


if(gbUtilsSTATIC)
  add_library(${LIB_NAME} STATIC
    ${gb_src_files} ${gb_header_files})
else()
  add_definitions(-DgbUtilsEXPORT)
  add_library(${LIB_NAME} SHARED
    ${gb_src_files} ${gb_header_files})
endif()

