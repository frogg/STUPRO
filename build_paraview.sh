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

# cmake config variables
PARAVIEW_CMAKE_VARIABLES="-D CMAKE_BUILD_TYPE=Release -D BUILD_TESTING=OFF -D Module_vtkGeovisCore=ON -D Module_vtkIOGeoJSON=ON -D Module_vtkViewsGeovis=ON"

# ensure bin_dir exists
if [ ! -d $BIN_DIR ];
then
  mkdir "$BIN_DIR"
fi
cd $BIN_DIR

# check if source folder already exists
if [ ! -d $PARAVIEW_SRC_DIR ];
then
  echo "Initializing Paraview repository"
  # clone paraview repository v4.3.1
  mkdir -p "$PARAVIEW_SRC_DIR"
  cd $PARAVIEW_SRC_DIR
  git init . && git remote add origin $PARAVIEW_GIT_URL
  cd ../
else
  echo "Paraview sources already exist"
fi

echo "Updating Paraview sources"
cd $PARAVIEW_SRC_DIR
git fetch origin && git checkout $PARAVIEW_TAG
# update paraview submodules
git submodule init
git submodule update
cd ../

echo "Building Paraview sources"
# create paraview-bin directory if it doesn't exist
if [ ! -d $PARAVIEW_BUILD_DIR ];
then
  mkdir -p "$PARAVIEW_BUILD_DIR"
fi

cd $PARAVIEW_BUILD_DIR
echo "Configuring Paraview"
cmake 1> /dev/null\
  $PARAVIEW_CMAKE_VARIABLES \
  ../$PARAVIEW_SRC_DIR

# build paraview
echo "Building Paraview"
make -j 4
