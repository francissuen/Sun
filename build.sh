#!/bin/bash

# Copyright (c) 2019, F.S.. All rights reserved.

help-msg()
{
    echo "-h: print this help message"
    echo "omit: build debug only"
    echo "-r: build release only"
    echo "-p: build both release and debug, then pkg"
    echo "-c: clean"
    echo "-g: generate build system only, cached cmake variables will be used"

}

if [[ ! -d "build" ]]
then
    mkdir build
fi

cd build

fs_cmake_flags="-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"

case $1 in
    -h)
        help-msg
        ;;
    -g)
        cmake ..
        ;;
    -r)
        cmake $fs_cmake_flags -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
        ;;
    -p)
        cmake $fs_cmake_flags -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .
        cmake $fs_cmake_flags -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
        ../fs_pkg.sh 
        ;;
    -c)
        rm -rfv ./*
        ;;
    *)
        cmake $fs_cmake_flags -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .
        ;;
esac
