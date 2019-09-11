#!/bin/bash
test $DEBUG && set -x

SFML_DEPENDENCIES="libudev-dev libfreetype6-dev libflac-dev"

mkdir -p _Build/ThirdParty
if [ -d BuildCache/ThirdParty ]; then
    echo "Restoring ThirdParty via BuildCache..."
    cp -r BuildCache/ThirdParty _Build
else
    echo "Building ThirdParty"
    sudo apt-get install -y $SFML_DEPENDENCIES

    # Build ThirdParty
    mkdir Project_ThirdParty
    cd Project_ThirdParty
    cmake -GNinja ../Source/ThirdParty
    ninja -v
    cd ..

    # Cache
    cp -r _Build/ThirdParty BuildCache/
fi
