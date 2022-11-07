#!/bin/bash

set -xe

mkdir -p build
rm -rf build/*
cd build
cmake ..
make


