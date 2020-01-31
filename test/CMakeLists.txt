# Copyright (C) 2020 Francis Sun, all rights reserved.

set(TEST_SUN "test_sun")

add_executable(${TEST_SUN})
target_include_directories(${TEST_SUN} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${TEST_SUN} ${SUN})
add_subdirectory(src)

add_dependencies(${TEST_SUN} ${SUN})
