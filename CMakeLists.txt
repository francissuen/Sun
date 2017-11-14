cmake_minimum_required(VERSION 3.0)
project(gbUtils)

include(gbCMake/gbCommon.cmake)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

option(gbLUAAPI "export lua api?" OFF)
option(gbUtilsSTATIC "build static or shared lib?" ON)

configure_file(${PROJECT_SOURCE_DIR}/gbUtilsConfig.h.in ${PROJECT_SOURCE_DIR}/gbUtilsConfig.h)

set(LIB_NAME "gbUtils")

set(gb_src_files)

gb_add_class(gbLog src gb_src_files)
gb_add_class(gbTime src gb_src_files)
gb_add_class(gbString src gb_src_files)
gb_add_class(gbThreadPool src gb_src_files)
gb_add_class(gbFileSystem src gb_src_files)

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/gbCMake/")
# find_package(lua REQUIRED)
# include_directories(SYSTEM ${lua_INCLUDE_DIRS})

gb_add_class(gbLCTypeConversion src gb_src_files)
gb_add_class(gbLuaCPP src gb_src_files)

include_directories(AFTER SYSTEM ${CMAKE_CURRENT_SOURCE_DIR})

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
# if(UNIX)
#   set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/gbUtils)
# endif()

message("install dir:" " ${CMAKE_INSTALL_PREFIX}")

install(TARGETS ${LIB_NAME} ARCHIVE DESTINATION gbUtils/lib)
# install(FILES ${gb_header_files} DESTINATION include)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/ DESTINATION gbUtils/include
  FILES_MATCHING PATTERN "*.h"
  )

