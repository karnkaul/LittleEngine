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
    mkdir Project_ThirdParty
    cd Project_ThirdParty
    cmake -GNinja ../Source/ThirdParty -DBUILD_SHARED_LIBS=1 -DCI_BUILD=1
    ninja -v
    cd ..

    # Cache
    cp -r _Build/CI BuildCache/
fi
