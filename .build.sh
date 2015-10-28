#!/bin/sh
mkdir -p bin
cd bin
cmake ../
make
make test
