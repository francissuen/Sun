#!/bin/bash

if ! [ -f ./test.sh ]
then
    echo "need run in test dir"
    exit 1
fi


cd ..				# cd to gbUtils dir

if ! [ -d build ]		# mk build dir of gbUtils
then
    mkdir build
fi


if [ "$1" = "-c" ]		# clean
then
    rm -rfv build/*
fi

cd test 			# cd to test dir

if ! [ -d build ]		# mk build dir of test
then
    mkdir build
fi

cd build			# cd to test build dir

if [ "$1" = "-c" ]		# clean
then
    rm -rfv build/* 
fi

cmake .. && cmake --build . && 	./gbtest -d -n hello -c 100 world fsgb # build and run

cd ..

exit 0
