#!/bin/bash

# for i in {1..10}
# do
#     ./ad ../../traces/memtrace-503.out 5 $i > ad_505_5_${i} &
# done
./ad ../../traces/memtrace-503.out 5 1 63.5 5 > ad_503_5_1 &
./ad ../../traces/memtrace-503.out 5 2 63.5 5 > ad_505_5_2 &
./ad ../../traces/memtrace-503.out 5 3 63.5 5 > ad_505_5_3 &
./ad ../../traces/memtrace-503.out 5 4 63.5 5 > ad_505_5_4 &
./ad ../../traces/memtrace-503.out 5 5 63.5 5 > ad_505_5_5 &
./ad ../../traces/memtrace-503.out 5 6 63.5 5 > ad_505_5_6 &
./ad ../../traces/memtrace-503.out 5 7 63.5 5 > ad_505_5_7 &
./ad ../../traces/memtrace-503.out 5 8 63.5 5 > ad_505_5_8 &
./ad ../../traces/memtrace-503.out 5 9 63.5 5 > ad_505_5_9 &
./ad ../../traces/memtrace-503.out 5 10 63.5 5 > ad_505_5_10 &
