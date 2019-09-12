#!/bin/bash
test $DEBUG && set -x
set -e

clang --version
cmake --version
ninja --version

# Build LittleEngine

build() {
	CONFIG=$1
	echo "Building LittleEngine | $CONFIG..."
	if [ ! -d Project_$CONFIG ]; then
		mkdir Project_$CONFIG
	fi
	cd Project_$CONFIG
	cmake -GNinja ../Source -DCMAKE_BUILD_TYPE=$CONFIG -DCI_BUILD=1 -DSFML_STATIC_LIBS=0
	ninja -v
	cmake --install .
	cd ..
}

# Ship
build Ship
build Develop
