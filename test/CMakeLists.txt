# Copyright (C) 2020 Francis Sun, all rights reserved.

set(SUN_TEST ${SUN}_test)

add_executable(${SUN_TEST} "")
target_include_directories(${SUN_TEST} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${SUN_TEST} ${SUN})
add_subdirectory(src)

add_dependencies(${SUN_TEST} ${SUN})
