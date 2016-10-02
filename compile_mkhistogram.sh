#!/bin/bash

# dynamic
export LD_LIBRARY_PATH=.
g++ -fpic -shared histolong.cpp -o libhistolong.so -O2 -Wall -Wno-unused-result -std=c++11  
g++ mkhistogram.cpp -L. -Wl,-R$PWD -lhistolong -o mkhistogram -O2 -Wall -Wno-unused-result -std=c++11 

