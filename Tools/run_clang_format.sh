#!/bin/bash

# This script runs clang-format on all Source files (except `ThirdParty`)
# Requirements:
#  - clang-format

EXCLUDE=*ThirdParty/*
BUILD=*out/*
RUN=FALSE
[ "$1" == "-r" ] && RUN=TRUE

THISDIR=$(pwd)
cd "$(dirname "${BASH_SOURCE[0]}")"
. ./os.sh
cd "$THISDIR"

FILES=$(find . -not -path "$EXCLUDE" -not -path "$BUILD" \( -name "*.h" -o -name "*.cpp" \))
COUNT=$(echo -e "$FILES" | wc -l)
echo -e "\nFiles:\n\n$FILES\n"

if [ "$RUN" == "TRUE" ]; then
	$CLANG_FORMAT -i $FILES
	echo "== Ran clang-format on $COUNT source files"
else
	echo "-- Pass '-r' to run clang-format on $COUNT source files"
fi

exit
