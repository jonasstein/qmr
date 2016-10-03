#!/bin/bash
for i in $( ls ~/mdat/*.mdat ); do
    b=$(basename $i)
    ./qmr -f $i > /tmp/$b.csv
    echo item: $i
done

