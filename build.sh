#!/bin/bash

set -e

config=$1

if [[ ! "$config" =~ ^(Debug|Release|RelWithDebInfo)$ ]]; then
    config=Debug
fi

mkdir -p build
mkdir -p build/$config
cd build/$config

cmake -DCMAKE_BUILD_TYPE=$config ../../
make -j $(nproc)

read -n 1 -p "Run program? " run_program
echo -e "\n"
case "$run_program" in
    n|N)
        echo "Not running."
        ;;
    *)
        echo "Running..."
        if [[ "$config" =~ ^(Debug|RelWithDebInfo)$ ]]; then
            gdb ./voice_changer -ex run
        else
            ./voice_changer
        fi
        ;;
esac
