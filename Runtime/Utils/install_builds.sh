#!/bin/bash

if [[ $# < 1 ]]; then
	echo "Syntax for ../../Project<X>: install_builds.sh <X0> [X1] [X2...]"
	exit 1
fi

if [[ "$(pwd)" == *"Runtime" ]]; then
	cd Utils
fi

## Confirm paths exist
for SUFFIX in "${@}"; do
	PROJ_DIR=Project$SUFFIX
	PROJ_PATH=../../$PROJ_DIR
	if [[ ! -d $PROJ_PATH ]]; then
		echo -e "\nError! [$PROJ_PATH] does not exist!"
		exit 1
	elif [[ ! -f $PROJ_PATH/CMakeCache.txt ]]; then
		echo -e "\nError! [$PROJ_PATH/CMakeCache.txt] does not exist!"
	fi
done

## MSVC Build
GENERATE=TRUE
MS=$(grep Microsoft /proc/version)
MW=$(grep MINGW /proc/version)
if [[ ! -z "$MS" ]] || [[ ! -z "$MW" ]]; then
	# Builds will fail on Win64 without running $VCVARS/vcvarsall.bat; so need a custom batch script
	GENERATE=FALSE
	BUILD_SCRIPT=build_win64.bat
	if [[ $(uname -s) == "Linux" ]]; then
		WSL=TRUE
		BUILD_CMD="cmd.exe /c "
	else
		BUILD_CMD="./"
	fi
	if [[ -f ./$BUILD_SCRIPT ]]; then
		echo -e "\n  Found local build script [$BUILD_SCRIPT], running before installing...\n"
		$BUILD_CMD$BUILD_SCRIPT $*
		if [[ $? != 0 ]]; then
			echo -e "\n  Build failed!"
			exit 1
		fi
	else
		echo -e "\n  ERROR: Win64 platform requires $BUILD_SCRIPT (not found)!"
		exit 1
	fi
fi

## Build and/or Install
for SUFFIX in "${@}"; do
	PROJ_DIR=Project$SUFFIX
	PROJ_PATH=../../$PROJ_DIR
	INSTALLING="\n== Installing [$PROJ_DIR]\n"
	if [[ $GENERATE == TRUE ]]; then
		echo -e "\n== Configuring [$PROJ_DIR]\n"
		cmake $PROJ_PATH
		if [[ -f "$PROJ_PATH/build.ninja" ]]; then
			echo -e "$INSTALLING"
			ninja install -C $PROJ_PATH
		elif [[ -f "$PROJ_PATH/Makefile" ]]; then
			echo -e "$INSTALLING"
			make install -C $PROJ_PATH
		else
			echo "\n ERROR: Install [$PROJ_DIR] failed! (Unsupported build system)"
		fi
	else
		echo -e "$INSTALLING"
		cmake --install $PROJ_PATH
	fi
done
