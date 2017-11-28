#!/bin/bash

if [ $EUID -ne 0 ]
then
    echo "need run as root"
    exit 1
fi


if ! [ -f ./test.sh ]
then
    echo "need run in test dir"
    exit 1
fi

origin_dir=$PWD


# build and install gbUtils
cd ..

if ! [ -d build ]
then
    mkdir build
fi

cd build

if [ "$1" = "-r" ]
then
    # rebuilding
    rm -rfv ./* && cmake .. && cmake --build . --target install
else
    cmake --build . --target install
fi



# build test
cd $origin_dir

if ! [ -d build ]
then
    mkdir build
fi

cd build

if [ "$1" = "-r" ]
then
    rm -rfv ./* && cmake .. && cmake --build .
else
    cmake --build .
fi

# run test
./gbtest -d -n hello -c 100 world fsgb

exit 0
