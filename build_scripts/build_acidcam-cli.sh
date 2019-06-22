#/usr/bin/env bash

mkdir -p build
cd build
git clone https://github.com/lostjared/acidcam-cli.git
cd acidcam-cli
mkdir -p build
cd build
cmake ..
make -j4
sudo make install
cd ..
cd ..
cd ..




