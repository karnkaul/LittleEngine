#!/bin/bash

# This script creates a nominal app bundle using the following:
#  - ../[Executable] ($1)
#  - ../Frameworks
#  - ../Lib
#  - ../Resources/Info.plist
#  - ../Resources/Icon.icns
#
# All files are copied, execpt the executable, which is moved
#
# Requirements:
#  - *nix (symlinks)

cd "$(dirname "${BASH_SOURCE[0]}")/.."

. ./Utils/os.sh

if [ "$IS_SYMLINKS" != "TRUE" ]; then
	echo "Error: OS does not support symlinks!"
    exit 1
fi

if [ -z $1 ] || [ ! -f $1 ]; then
    echo -e "Missing executable/filename!"
    exit 1
fi

check() 
{
    if [ ! $1 $2 ]; then
        echo -e "Missing $2!"
        exit 1
    fi
}

check -f Resources/Info.plist
check -d Frameworks
check -d Lib

[ -d $1.app ] && rm -rf $1.app
chmod a+x $1
mkdir -p $1_/Contents/MacOS
cp $1 $1_/Contents/MacOS/$1
cp Resources/Info.plist $1_/Contents/
cp -r Frameworks $1_/Contents/MacOS/
cp -r Lib $1_/Contents/MacOS/
if [ -f Resources/Icon.icns ]; then
    mkdir -p $1_/Contents/Resources 
    cp Resources/Icon.icns $1_/Contents/Resources/
fi
mv $1_ $1.app
