#!/bin/bash
export LD_LIBRARY_PATH=~/boost_1_66_0/stage/lib/
mkdir build
cd build
export CXX=/usr/bin/clang++-4.0
cmake ..
cmake -DCMAKE_INSTALL_PREFIX:PATH=install ..
make all
make install
cd install
./bin/geogen
