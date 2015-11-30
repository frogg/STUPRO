#!/bin/sh
BUILD_DIR="$PWD/bin/gtest/"
SRC_DIR="$PWD/libs/gtest/"

CMAKE_FLAGS="\
 -D BUILD_SHARED_LIBS=ON\
 -D BUILD_GTEST=ON\
 -D BUILD_GMOCK=OFF"

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake $CMAKE_FLAGS $SRC_DIR > /dev/null
if [ $? != 0 ]; then
  echo "Aborting GTest build due to cmake errors"
  exit 1
fi

make
if [ $? != 0 ]; then
  echo "Error building GTest, aborting"
  exit 1
fi

echo "Successfully built GTest"
