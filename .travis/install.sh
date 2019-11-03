#!/bin/bash
test $DEBUG && set -x

## This script removes existing CMake (usually 3.12 on Ubuntu) and installs the latest:
##   - cmake
##   - g++-8
##   - ninja-build
##   - clang-8, lld-8
##   - SFML dependencies

ENV="cmake g++-8 ninja-build clang libx11-dev libxrandr-dev libgl1-mesa-dev libudev-dev libfreetype6-dev libflac-dev libopenal-dev libvorbis-dev"

# Get latest keys for cmake, g++, etc
# clang-8
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key 2>/dev/null | sudo apt-key add -
# cmake 3.15
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' -y
# g++-8
sudo -E apt-add-repository -y "ppa:ubuntu-toolchain-r/test"

# Purge all existing cmake/clang installations
sudo apt-get purge {cmake,clang,lld}
sudo rm -rf /usr/{bin/cmake*,share/cmake*,local/bin/cmake*,local/cmake*}
[[ "$CLANG" == "TRUE" ]] &&	sudo rm -rf /usr/{bin/clang*,share/clang*,local/bin/clang*}

# Install dependencies
sudo apt-get update
sudo apt-get install -y $ENV

exit
