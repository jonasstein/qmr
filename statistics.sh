#!/bin/bash

echo collect statistics on converted listmode files
echo "count  channel"
cat $1 | cut -f 3 -d" " | sort | uniq -c
