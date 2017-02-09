#!/bin/bash

# dynamic
export LD_LIBRARY_PATH=.
g++ slicemdat.cpp -o slicemdat -O2 -Wall -Wno-unused-result -std=c++11 

