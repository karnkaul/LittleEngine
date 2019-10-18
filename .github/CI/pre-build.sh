#!/bin/bash

ENV="cmake g++-8 ninja-build libx11-dev libxrandr-dev libgl1-mesa-dev"

# Get latest keys for cmake, g++, etc
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' -y
sudo -E apt-add-repository -y "ppa:ubuntu-toolchain-r/test"

# Purge all existing cmake installations
sudo apt-get purge cmake
sudo apt-get update
sudo rm -rf /usr/bin/cmake*
sudo rm -rf /usr/share/cmake*
sudo rm -rf /usr/local/bin/cmake*

# Install dependencies
sudo apt-get install -y $ENV

#!/bin/bash
test $DEBUG && set -x

SFML_DEPENDENCIES="libudev-dev libfreetype6-dev libflac-dev"

BUILD_TP=TRUE
mkdir -p _Build
if [ -d BuildCache/CI ]; then
    if [ ! -z ${REBUILD_CACHE} ]; then
        echo "== Rebuilding ThirdParty..."
    else
        echo "== Restoring ThirdParty via BuildCache..."
        cp -r BuildCache/CI _Build
        BUILD_TP=FALSE
    fi
else
    echo "== Building ThirdParty..."
fi

if [ "$BUILD_TP" == "TRUE" ]; then
    sudo apt-get install -y $SFML_DEPENDENCIES
    OUT=out/TP
    mkdir -p $OUT
    cmake -GNinja ../Source/ThirdParty -B$OUT -DBUILD_SHARED_LIBS=1 -DCI_BUILD=1
    ninja -v -C$OUT

    # Cache
    cp -r _Build/CI BuildCache/
fi

exit
