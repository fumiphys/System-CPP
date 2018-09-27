#!/bin/bash -eu
# install script
# author: fumiphys
# type `./install.sh`

mkdir -p build && cd build
cmake ..
make
cp ../Network/index.html .
