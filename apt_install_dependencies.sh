#!/bin/bash

CMD=install

if [[ $1 == "-u" ]] || [[ $1 == "--uninstall" ]]; then
	CMD=remove
	BUILD_SFML=y
else
	read -p "Building SFML? [y]: " -rn1 BUILD_SFML
fi

# SFML build time
if [[ $BUILD_SFML == "y" ]]; then
	SFML_BUILD="freeglut3-dev libudev-dev libfreetype6-dev libvorbis-dev libflac-dev"
fi

# SFML link / load time
SFML="$SFML_BUILD libx11-dev libxrandr-dev libopenal-dev"

# LittleEngine
if [[ $CMD == "install" ]]; then
	LE="cmake cmake-gui git git-lfs ninja-build clang llvm"
fi

apt $CMD --assume-yes $SFML $LE

if [[ $CMD == "remove" ]]; then
	apt autoremove --assume-yes
fi