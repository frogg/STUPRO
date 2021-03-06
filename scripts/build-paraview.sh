#!/bin/sh
PV_SRC_DIR="./bin/paraview/src"
PV_BIN_DIR="./bin/paraview/bin"
PV_CMAKE_FLAGS="\
 -D CMAKE_BUILD_TYPE=Release\
 -D BUILD_TESTING=OFF\
 -D BUILD_DOCUMENTATION=OFF\
 -D PARAVIEW_USE_MPI=ON"

PV_GIT_URL="https://github.com/Kitware/ParaView.git"
PV_GIT_TAG="v4.3.1"

NUM_WORKERS=$(nproc)

# store current working directory for later references
ROOT_DIR=$PWD
ABS_PV_SRC_DIR="$PWD/$PV_SRC_DIR"
ABS_PV_BIN_DIR="$PWD/$PV_BIN_DIR"

# create ParaView source code directory if it doesn't exist and check if it already is on the correct tag
mkdir -p $ABS_PV_SRC_DIR
cd $ABS_PV_SRC_DIR

CURRENT_PV_GIT_TAG=$(git describe --tag)
if [ "$CURRENT_PV_GIT_TAG" = "$PV_GIT_TAG" ]; then
  echo "Paraview already seems to be on the correct tag, skipping clone"
else
  echo "Cloning ParaView Git repository..."
  git clone $PV_GIT_URL . 2> /dev/null
  git checkout $PV_GIT_TAG

  # initialize and update submodules required by ParaView
  git submodule init
  git submodule update
fi

# if the PV_CLEAN_BUILD environment variable is set, clear the build directory first
if [ $PV_CLEAN_BUILD ]; then
  echo "PV_CLEAN_BUILD is set, cleaning ParaView build directory..."
  rm -rf $ABS_PV_BIN_DIR
else
  # check if the ParaView build directory already exists
  if [ $PV_REBUILD ]; then
    echo "PV_REBUILD is set"
  else
    if [ -d $ABS_PV_BIN_DIR ]; then
      cd $ROOT_DIR
      echo "$ABS_PV_BIN_DIR already exists, assuming ParaView binaries are already built"
      exit 0
    fi
  fi
fi

# descend into PV build directory
mkdir -p $ABS_PV_BIN_DIR
cd $ABS_PV_BIN_DIR

# configure paraview
echo "Configuring ParaView..."
cmake $PV_CMAKE_FLAGS $ABS_PV_SRC_DIR > /dev/null

# make sure cmake didn't fail
if [ $? != 0 ]; then
  cd $ROOT_DIR
  echo "Aborting ParaView build due to cmake errors"
  exit 1
fi

echo "Building ParaView in $PWD..."
make -j $NUM_WORKERS

if [ $? != 0 ]; then
  cd $ROOT_DIR
  echo "Error building ParaView, aborting"
  exit 1
fi

cd $ROOT_DIR
echo "Successfully built ParaView"
