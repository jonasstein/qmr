#!/bin/bash

# dynamic
export LD_LIBRARY_PATH=.
g++ syncinfo.cpp -L. -Wl,-R$PWD -o syncinfo -O2 -Wall -Wno-unused-result -std=c++11 

