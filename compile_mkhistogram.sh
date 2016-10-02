#!/bin/bash

# dynamic
export LD_LIBRARY_PATH=.
g++ -fpic -shared histogram.cpp -o libhistogram.so -O2 -Wall -Wno-unused-result -std=c++11  
g++ mkhistogram.cpp -L. -Wl,-R$PWD -lhistogram -o mkhistogram -O2 -Wall -Wno-unused-result -std=c++11 

