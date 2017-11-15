cmake_minimum_required(VERSION 3.0)
project(gbUtils)

include(gbCMake/gbCommon.cmake)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

option(gbLUAAPI "export lua api?" OFF)
option(gbUtilsSTATIC "build static or shared lib?" ON)

configure_file(${PROJECT_SOURCE_DIR}/gbUtilsConfig.h.in ${PROJECT_SOURCE_DIR}/gbUtilsConfig.h)

set(LIB_NAME "gbUtils")

gb_add_class(common src gb_src_files)
gb_add_class(logger src gb_src_files)
gb_add_class(time src gb_src_files)
gb_add_class(string src gb_src_files)
gb_add_class(threadpool src gb_src_files)
gb_add_class(filesystem src gb_src_files)

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/gbCMake/")
# find_package(lua REQUIRED)
# include_directories(SYSTEM ${lua_INCLUDE_DIRS})

gb_add_class(luacpp_type_conversion src gb_src_files)
gb_add_class(luacpp src gb_src_files)
gb_add_class(args src gb_src_files)


include_directories(AFTER SYSTEM ${CMAKE_CURRENT_SOURCE_DIR})

if(gbUtilsSTATIC)
  add_library(${LIB_NAME} STATIC
    ${gb_src_files}
    )
else()
  add_definitions(-DgbUtilsEXPORT)
  add_library(${LIB_NAME} SHARED
    ${gb_src_files}
    )
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

set(headers)
foreach(src IN LISTS gb_src_files)
  if(${src} MATCHES ".*\\.h") 
    set(headers ${headers} ${src})
    message("header: " ${src})
  endif()
endforeach()
set(headers ${headers} gbUtilsConfig.h)

install(FILES ${headers} DESTINATION gbUtils/include)
