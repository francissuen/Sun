#!/bin/bash

if [ $EUID -ne 0 ]
then
    echo "should run as su"
    exit 1
fi

if ! [ -f ./install.sh ]
then
    echo "should run in the same dir as install.sh"
    exit 1
fi

if ! [ -d ./build ]		# mk build dir
then
    mkdir build
fi

cd build

if [ "$1" = "-c" ]		# clean
then
    rm -rfv ./*
fi

cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . --target install # install debug version

cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . --target install # install release version

