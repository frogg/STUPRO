#!/bin/sh

# bin-folder (all builds and external dependencies will be put there)
BIN_DIR="./bin"

# where to clone the paraview source files to (inside BIN_DIR)
PARAVIEW_SRC_DIR="paraview-src"
# where to build paraview (inside BIN_DIR)
PARAVIEW_BUILD_DIR="paraview-bin"

# paraview repo url
PARAVIEW_GIT_URL="https://github.com/Kitware/ParaView.git"
# paraview tag to checkout
PARAVIEW_TAG="v4.3.1"

# number of cores to use when running make
NUM_CORES=6

# cmake config variables
PARAVIEW_CMAKE_VARIABLES="-D CMAKE_BUILD_TYPE=Release -D BUILD_TESTING=OFF -D Module_vtkGeovisCore=ON -D Module_vtkIOGeoJSON=ON -D Module_vtkViewsGeovis=ON"

# debug output
# TODO: remove
echo "looking for xmlpatterns"
find / -iname xmlpatterns


# ensure bin_dir exists
if [ ! -d $BIN_DIR ];
then
  mkdir "$BIN_DIR"
fi
cd $BIN_DIR

# make sure source folder already exists
if [ ! -d $PARAVIEW_SRC_DIR ];
then
  echo "Creating Paraview source folder"
  mkdir -p "$PARAVIEW_SRC_DIR"
fi

# make sure git repository is initialized
if [ ! -d $PARAVIEW_SRC_DIR/.git ];
then
  echo "Initializing Paraview repository"
  cd $PARAVIEW_SRC_DIR
  git init . && git remote add origin $PARAVIEW_GIT_URL
  # return to bin_dir
  cd ../
fi

# update paraview repository
echo "Updating Paraview sources"
cd $PARAVIEW_SRC_DIR
git fetch origin && git checkout $PARAVIEW_TAG
# update paraview submodules
git submodule init
git submodule update
# return to bin_dir
cd ../

# make sure paraview-bin directory exists
if [ ! -d $PARAVIEW_BUILD_DIR ];
then
  echo "Creating directory $PARAVIEW_BUILD_DIR"
  mkdir -p "$PARAVIEW_BUILD_DIR"
fi

cd $PARAVIEW_BUILD_DIR
echo "Configuring Paraview"
# pipe stdout to /dev/null in order to clean up the build log
cmake 1> /dev/null\
  $PARAVIEW_CMAKE_VARIABLES \
  ../$PARAVIEW_SRC_DIR

# build paraview (finally)
echo "Building Paraview"
make -j $NUM_CORES
