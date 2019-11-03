#!/bin/bash
test $DEBUG && set -x
set -e

clang --version; cmake --version; ninja --version

# Build LittleEngine
build() {
	CONFIG=$1
	echo "== Building LittleEngine | $CONFIG..."
	[[ ! -d out/$CONFIG ]] && mkdir -p out/$CONFIG
	cmake -G Ninja $2 -B $3/$CONFIG -DCMAKE_BUILD_TYPE=$CONFIG -DUPDATE_SUBMODULES=0 -DCMAKE_CXX_COMPILER=clang -DCMAKE_C_COMPILER=clang
	cmake --build out/$CONFIG
}

[[ -z "$CONFIGS" ]] && CONFIGS=Release

for CONFIG in $CONFIGS; do
	build $CONFIG $1 $2
done
