#!/bin/bash

# dynamic
export LD_LIBRARY_PATH=.
g++ filter.cpp -o filter -O2 -Wall -Wno-unused-result -std=c++11 

