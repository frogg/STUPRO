#!/bin/sh
BUILD_DIR="$PWD/bin/gtest/"
SRC_DIR="$PWD/libs/gtest/"

CMAKE_FLAGS="\
 -D BUILD_SHARED_LIBS=ON\
 -D BUILD_GTEST=ON\
 -D BUILD_GMOCK=OFF"

LIBRARAY_PATH="googletest/libgtest.so"

if [ $GTEST_CLEAN_BUILD ]; then
  echo "GTEST_CLEAN_BUILD is set, cleaning GTest build directory..."
  rm -rf $BUILD_DIR
else
  if [ $GTEST_REBUILD ]; then
    echo "GTEST_REBUILD is set"
  else
    if [ -f $BUILD_DIR$LIBRARAY_PATH ]; then
      echo "$BUILD_DIR$LIBRARAY_PATH already exists, assuming GTest libraries are already built"
      exit 0
    fi
  fi
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake $CMAKE_FLAGS $SRC_DIR > /dev/null
if [ $? != 0 ]; then
  echo "Aborting GTest build due to cmake errors"
  exit 1
fi

make -j $(nproc)
if [ $? != 0 ]; then
  echo "Error building GTest, aborting"
  exit 1
fi

echo "Successfully built GTest"
