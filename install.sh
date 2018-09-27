#!/bin/bash -eu
# install script
# author: fumiphys
# type `./install.sh`

if [ ! -e Network/https_server.key ];then
  cd Network
  chmod +x certificate.sh
  ./certificate.sh
  cd ..
fi

mkdir -p build && cd build
cmake ..
make clean
make
cp ../Network/index.html .
cp ../Network/https_server.key .
cp ../Network/https_server.crt .
cd ..

mkdir -p test/c_server/files
cp Network/index.html test/c_server/files/
cp build/Network/http_server test/c_server/files/
cp build/Network/https_server test/c_server/files/
cp build/Network/socket_server test/c_server/files/
