#!/bin/bash

# static compile

#gcc -c histogram.cpp -o histogram.o -std=c++11
#ar cr libhistogram.a histogram.o

#gcc -c lmfile.cpp -o lmfile.cpp -std=c++11
#ar cr liblmfile.a lmfile.o

#gcc dump.cpp -L. -lhistogram -llmfile  -std=c++11

# dynamic
export LD_LIBRARY_PATH=.
g++ -fpic -shared histogram.cpp -o libhistogram.so -std=c++11
g++ -fpic -shared lmfile.cpp -L. -lhistogram -o liblmfile.so -std=c++11 
g++ dump.cpp -L. -Wl,-R$PWD -llmfile -lhistogram -o dump -std=c++11

# g++ lmfile.cpp -O2 -Wall -Wno-unused-result -std=c++11 -ggdb -c

#g++ lmfile.cpp histogram.cpp -O2 -Wall -Wno-unused-result -std=c++11 -ggdb -c
#g++ histogram.cpp lmfile.cpp  -O2 -Wall -Wno-unused-result -std=c++11 -ggdb -c

#g++ -o qmdump lmfile.cpp histogram.cpp dump.cpp -O2 -Wall -Wno-unused-result -std=c++11 -ggdb
