#!/bin/bash
test $DEBUG && set -x

SFML_DEPENDENCIES="libudev-dev libfreetype6-dev libflac-dev"

mkdir -p _Build
if [ -d BuildCache/CI ]; then
    echo "== Restoring ThirdParty via BuildCache..."
    cp -r BuildCache/CI _Build
else
    echo "== Building ThirdParty"
    sudo apt-get install -y $SFML_DEPENDENCIES
    mkdir Project_ThirdParty
    cd Project_ThirdParty
    cmake -GNinja ../Source/ThirdParty -DBUILD_SHARED_LIBS=1 -DCI_BUILD=1
    ninja -v
    cd ..

    # Cache
    cp -r _Build/CI BuildCache/
fi
