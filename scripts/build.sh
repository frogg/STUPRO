#!/bin/sh
mkdir -p bin
cd bin
cmake ../
make
ctest --output-on-failure
