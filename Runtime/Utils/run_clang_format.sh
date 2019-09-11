#!/bin/bash

SOURCE_ROOT=../..
SOURCE_PATH=$SOURCE_ROOT/Source
EXCLUDE=*ThirdParty/*

if [[ "$(pwd)" == *"Runtime" ]]; then
	cd Utils
fi

if [[ ! -d $SOURCE_PATH ]]; then
	echo -e "Error! $SOURCE_PATH does not exist!"
	exit 1
fi

FILES=$(find $SOURCE_PATH -not -path "$EXCLUDE" \( -name "*.h" -o -name "*.cpp" \))

clang-format -i $FILES
echo "Ran clang-format on $SOURCE_PATH (except ThirdParty)..."
