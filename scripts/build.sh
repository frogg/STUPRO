#!/bin/sh
ROOT_DIR=$PWD
BUILD_DIR=bin
ABS_BUILD_DIR=$ROOT_DIR/$BUILD_DIR
ABS_PV_DIR=$ROOT_DIR/bin/paraview/bin/

mkdir -p $ABS_BUILD_DIR
cd $ABS_BUILD_DIR
cmake -D ParaView_DIR=$ABS_PV_DIR $ROOT_DIR

if [ $? != 0 ]; then
  echo "Aborting due to errors executing cmake"
  exit 1
fi

make -j 4

if [ $? != 0 ]; then
  echo "Aborting due to errors while building kronos"
  exit 1
fi

ctest --output-on-failure
