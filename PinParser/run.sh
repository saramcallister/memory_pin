#!/bin/bash

for i in {1..10}
do
    ./ad ../../traces/memtrace-505.out 5 $i > ad_505_5_${i} &
done
