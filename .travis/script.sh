#!/bin/bash
test $DEBUG && set -x
set -e

clang --version; cmake --version; ninja --version

# Build LittleEngine
build() {
	CONFIG=$1
	echo "== Building LittleEngine | $CONFIG..."
	[[ ! -d out/$CONFIG ]] && mkdir -p out/$CONFIG
	cmake -G Ninja Source -Bout/$CONFIG -DCI_BUILD=1 -DCMAKE_BUILD_TYPE=$CONFIG -DSFML_STATIC_LIBS=0
	ninja -v -C out/$CONFIG
	ninja install -C out/$CONFIG
}

[[ -z "$CONFIGS" ]] && CONFIGS=Release

for CONFIG in $CONFIGS; do
	build $CONFIG
done
