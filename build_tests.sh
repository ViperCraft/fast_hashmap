#!/bin/bash

set -xe

mkdir -p build
rm -rf build/*
cd build
if [ $(which cmake3) ]; then
  cmake3 ..
else
  cmake ..
fi 
make


