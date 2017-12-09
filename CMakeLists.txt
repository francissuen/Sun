cmake_minimum_required(VERSION 3.0)
project(gbUtils)

include(gbCMake/gbCommon.cmake)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

option(gbLUAAPI "export lua api?" OFF)
option(gbUtilsSTATIC "build static or shared lib?" ON)

configure_file(${PROJECT_SOURCE_DIR}/gbUtilsConfig.h.in ${PROJECT_SOURCE_DIR}/src/gbUtilsConfig.h)

set(LIB_NAME "gbUtils")

gb_add_class(common src gb_src_files)
gb_add_class(logger src gb_src_files)
gb_add_class(time src gb_src_files)
gb_add_class(string src gb_src_files)
gb_add_class(concurrency src gb_src_files)
gb_add_class(filesystem src gb_src_files)

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/gbCMake/")
# find_package(lua REQUIRED)
# include_directories(SYSTEM ${lua_INCLUDE_DIRS})

# gb_add_class(luacpp_type_conversion src gb_src_files)
# gb_add_class(luacpp src gb_src_files)
gb_add_class(args src gb_src_files)
gb_add_class(file src gb_src_files)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(gbUtilsSTATIC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGB_UTILS_STATIC_LIB=1")
  add_library(${LIB_NAME} STATIC
    ${gb_src_files}
    )
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGB_UTILS_SHARED_LIB=1")
  add_library(${LIB_NAME} SHARED
    ${gb_src_files}
    )
endif()

set_target_properties(${LIB_NAME}
  PROPERTIES
  DEBUG_OUTPUT_NAME ${LIB_NAME}d
  RELEASE_OUTPUT_NAME ${LIB_NAME}
  )

set(CMAKE_INSTALL_PREFIX ${GB_DEFAULT_INSTALL_DIR}/gbUtils)


install(TARGETS ${LIB_NAME} ARCHIVE DESTINATION .)

set(headers)
foreach(src IN LISTS gb_src_files)
  if(${src} MATCHES ".*\\.h") 
    set(headers ${headers} ${src})
    message("header: " ${src})
  endif()
endforeach()
set(headers ${headers} src/gbUtilsConfig.h)

install(FILES ${headers} DESTINATION .)
