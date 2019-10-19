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
BRANCHES=(master dev stable)

# Read-only
BUILD_ROOT=_Build/CI
BATCH_LOCATION=Runtime/Utils
BATCH_EXIT="../.."
BATCH_SCRIPT=build_win64.bat
BUILD=FALSE

# cd to `LittleEngine`
cd "$(dirname "${BASH_SOURCE[0]}")/../.."
. Runtime/Utils/os.sh

# Early exit if no build directories exist
[ ! -d $BUILD_ROOT ] && exit 0

# Check if current branch is in BRANCHES
BRANCH=$(git branch | grep "*")
for ACTIVE in ${BRANCHES[@]}; do
	[[ "$BRANCH" == *"$ACTIVE" ]] && BUILD=TRUE
done
[[ "$1" == "-f" || "$1" == "--force" ]] && BUILD=TRUE
[ $BUILD == FALSE ] && exit 0

fatal_error() {
	echo "Fatal error! Aborting..."
	exit 1
}

build() {
	if [ -d $BUILD_ROOT/$1 ]; then
		echo -e "  == $1 Build [$THREADS threads]"
		if [ -z $2 ]; then
			$CMAKE $BUILD_ROOT/$1 -DCI_BUILD=1 > /dev/null || fatal_error
			$CMAKE --build $BUILD_ROOT/$1 -j$THREADS > /dev/null || fatal_error
		else
			cd $BATCH_LOCATION
			$RUN_BATCH$BATCH_SCRIPT $BATCH_EXIT/$BUILD_ROOT/$1 > /dev/null || fatal_error
			cd $BATCH_EXIT
		fi
	fi
}

echo "  Running local CI..."
# Build each subdirectory here
build LE_MinGW
#build LE_clang-cl CMD

echo -e "  Passed"

exit
