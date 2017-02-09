#!/bin/bash

echo collect statistics on converted listmode files
echo "reading file: $1"
echo "size: $(( $( stat -c '%s' $1 ) / 1024 / 1024 )) MB"


STARTTIME=`head $1 -n 1 | awk '{printf "%d", $1/1000000000}'`
ENDTIME=`tail $1 -n 1 | awk '{printf "%d", $1/1000000000}'` 

echo "$(($ENDTIME - $STARTTIME)) s complete length"

echo "--------------"
echo "count  channel"
cut -f 3 -d" " $1 | sort | uniq -c

echo -e "\n\nTimes of the first 10 Ch1 events"
grep " 7 1 " $1 | head | awk '{printf "%d ms \n", $1/1000000}'

