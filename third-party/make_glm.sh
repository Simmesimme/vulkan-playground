#!/bin/bash

# ------------------------------------------------------------------------------
# This script executes cmake, make and make install.
# ------------------------------------------------------------------------------

# get the location of this script
SCRIPT_DIR="$( cd "$( dirname "$0" )" && pwd )"
BUILD_TYPE=${1:-Release}

# define locations for build and install
CMAKE_DIR="$SCRIPT_DIR/glm"
BUILD_DIR="$SCRIPT_DIR/build/linux/$BUILD_TYPE/glm"
INSTALL_DIR="$SCRIPT_DIR/install/linux/$BUILD_TYPE"

# create build directory if neccessary -----------------------------------------

if [ ! -d $BUILD_DIR ]; then
  mkdir -p $BUILD_DIR
fi

# configure cmake --------------------------------------------------------------

cd $BUILD_DIR
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR -DCMAKE_BUILD_TYPE=$BUILD_TYPE $CMAKE_DIR

# compilation & installation ---------------------------------------------------

make -j8 install
