#!/bin/bash
test $DEBUG && set -x

SFML_DEPENDENCIES="libudev-dev libfreetype6-dev libflac-dev"

mkdir -p _Build/ThirdParty
if [ -d BuildCache/ThirdParty ]; then
    echo "Restoring ThirdParty via BuildCache..."
    cp -r BuildCache/ThirdParty _Build
else
    sudo apt-get install -y $SFML_DEPENDENCIES
    echo "Building ThirdParty"
    mkdir Project_ThirdParty
    cd Project_ThirdParty
    cmake -GNinja ../Source/ThirdParty -DBUILD_SHARED_LIBS=1
    ninja -v
    cd ..

    # Cache
    cp -r _Build/ThirdParty BuildCache/
fi
