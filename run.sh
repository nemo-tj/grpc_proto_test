#!/usr/bin
# https://www.cnblogs.com/umichan0621/p/16501843.html 
set -x
bash grpctool.sh
mkdir -p build/
cd build
cmake ..
make


