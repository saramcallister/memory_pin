#!/bin/bash
pages=("55812" "43002" "26197" "50987" "491174")
scale=("63.5" "30" "44" "35" "102.8")
pages=("111625" "86004" "52394" "101975" "982349")

# ./dynamic_lfu_hw ../traces/memtrace-503.out gt/dlfu_503 63.5 55812 111625 &
./dynamic_lfu_hw ../traces/memtrace-505.out hw/dlfu_505 30 43002 86004 &
./dynamic_lfu_hw ../traces/memtrace-519.out hw/dlfu_519 44 26197 52394 &
./dynamic_lfu_hw ../traces/memtrace-557cld.out hw/dlfu_557 35 50987 101975 &
./dynamic_lfu_hw ../traces/memtrace-602.out hw/dlfu_602 102.8 491174 982349 &
