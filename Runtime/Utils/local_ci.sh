#!/bin/bash

# This script is meant to be used as a pre-commit/pre-push hook,
# offering a local CI guard against broken build commits.
#
# Set up projects in `_Build/CI` with `CI_BUILD=1` and call 
# 	`build <proj_dir_name> [CMD]`
# Pass CMD for non MinGW projects.
#
# Requirements:
# - `cmake` in $PATH
# - Runtime/Utils/build_win64.bat (for non-MinGW builds)

# Globals
THREADS=8
BRANCHES=(master dev)

# Read-only
BUILD_ROOT=_Build/CI
BUILD_SCRIPT=Runtime/Utils/build_win64.bat
BUILD=FALSE

# Early exit if no build directories exist
[ ! -d $BUILD_ROOT ] && exit 0

# cd to `LittleEngine`
cd "$(dirname "${BASH_SOURCE[0]}")/../.."

# Check if current branch is in BRANCHES
BRANCH=$(git branch | grep "*")
for ACTIVE in ${BRANCHES[@]}; do
	[[ "$BRANCH" == "* $ACTIVE" ]] && BUILD=TRUE
done
[ $BUILD == FALSE ] && exit 0

fatal_error() {
	echo "Fatal error! Aborting..."
	exit 1
}

build() {
	if [ -d $BUILD_ROOT/$1 ]; then
		echo -e "  == $1 Build [$THREADS threads]"
		if [ -z $2 ]; then
			cmake $BUILD_ROOT/$1 -DCI_BUILD=1 > /dev/null || fatal_error
			cmake --build $BUILD_ROOT/$1 -j$THREADS > /dev/null || fatal_error
		else
			./$BUILD_SCRIPT $BUILD_ROOT/$1 > /dev/null || fatal_error
		fi
	fi
}

echo "  Running local CI..."
# Build each subdirectory here
build LE_MinGW
build LE_clang-cl CMD

echo -e "  Passed"
