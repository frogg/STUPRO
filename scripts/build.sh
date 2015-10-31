#!/bin/sh
mkdir -p bin
cd bin
cmake ../
make -j 4
ctest --output-on-failure
