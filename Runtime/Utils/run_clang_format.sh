#!/bin/bash

# This script runs clang-format on all Source files (except `ThirdParty`)
# Requirements:
#  - clang-format

SOURCE_ROOT=..
SOURCE_PATH=$SOURCE_ROOT/Source
EXCLUDE=*ThirdParty/*


cd "$(dirname "${BASH_SOURCE[0]}")/.."
. ./Utils/os.sh

if [[ ! -d $SOURCE_PATH ]]; then
	echo -e "Error! $SOURCE_PATH does not exist!"
	exit 1
fi

FILES=$(find $SOURCE_PATH -not -path "$EXCLUDE" \( -name "*.h" -o -name "*.cpp" \))

$CLANG_FORMAT -i $FILES
echo "Ran clang-format on $SOURCE_PATH (except ThirdParty)..."

exit
