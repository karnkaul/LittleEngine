#!/bin/bash
test $DEBUG && set -x

# Build Cache
if [ ! -d "BuildCache" ]; then
    mkdir BuildCache
else
    CACHE=$(ls -R BuildCache)
    echo -e "== BuildCache contents:\n$CACHE"
fi

# Dependencies
ENV="cmake g++-8 ninja-build libx11-dev libxrandr-dev libopenal-dev libvorbis-dev freeglut3-dev"

# CMake
sudo rm -rf /usr/local/cmake*
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ xenial main' -y

# g++
sudo -E apt-add-repository -y "ppa:ubuntu-toolchain-r/test"

sudo apt-get update
sudo apt-get install -y $ENV
