#!/bin/sh
mkdir -p bin
cd bin
cmake ../

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
