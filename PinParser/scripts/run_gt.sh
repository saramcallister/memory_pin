#!/bin/bash
pages=("55812" "43002" "26197" "50987" "491174")
scale=("63.5" "30" "44" "35" "102.8")
pages=("111625" "86004" "52394" "101975" "982349")

# ./dynamic_lfu ../../traces/memtrace-503.out gt/dlfu_503 63.5 55812 111625
./dynamic_lfu ../../traces/memtrace-505.out gt/dlfu_505 30 43002 86004 &
./dynamic_lfu ../../traces/memtrace-519.out gt/dlfu_519 44 26197 52394 &
./dynamic_lfu ../../traces/memtrace-557cld.out gt/dlfu_557 35 50987 101975 &
./dynamic_lfu ../../traces/memtrace-602.out gt/dlfu_602 102.8 491174 982349 &

./dynamic_lfu_fifo ../../traces/memtrace-503.out gt/dlfu_fifo_503 63.5 55812 111625 &
./dynamic_lfu_fifo ../../traces/memtrace-505.out gt/dlfu_fifo_505 30 43002 86004 &
./dynamic_lfu_fifo ../../traces/memtrace-519.out gt/dlfu_fifo_519 44 26197 52394 &
./dynamic_lfu_fifo ../../traces/memtrace-557cld.out gt/dlfu_fifo_557 35 50987 101975 &
./dynamic_lfu_fifo ../../traces/memtrace-602.out gt/dlfu_fifo_602 102.8 491174 982349 &
