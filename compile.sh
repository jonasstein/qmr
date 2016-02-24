#!/bin/bash

g++ lmfile.cpp -O2 -Wall -Wno-unused-result -std=c++11 -ggdb -c
g++ -o qmread lmfile.cpp main.cpp -O2 -Wall -Wno-unused-result -std=c++11 -ggdb
