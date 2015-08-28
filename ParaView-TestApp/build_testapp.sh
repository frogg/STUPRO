#!/bin/sh

# directory in which to perform the build-process
BUILD_DIR="bin"
# directory in which paraview was BUILD_DIR
PARAVIEW_DIR="../../bin/paraview-bin"

# make sure the build folder exists
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# run configuration
cmake -D ParaView_DIR="$PARAVIEW_DIR" ../

# build testapp
make
