#!/bin/bash

./lfu_pebs ../../traces/memtrace-503.out new_pebs_503.out 1024 > pebs_out/debug_lfu_503
./lfu_pebs ../../traces/memtrace-505.out new_pebs_505.out 1024 > pebs_out/debug_lfu_505
./lfu_pebs ../../traces/memtrace-519.out new_pebs_519.out 1024 > pebs_out/debug_lfu_519
./lfu_pebs ../../traces/memtrace-557cld.out new_pebs_557cld.out 1024 > pebs_out/debug_lfu_557cld
./lfu_pebs ../../traces/memtrace-602.out new_pebs_602.out 1024 > pebs_out/debug_lfu_602

./random_pebs ../../traces/memtrace-503.out new_pebs_503.out pebs_out/debug_random_503 1024 
./random_pebs ../../traces/memtrace-505.out new_pebs_505.out pebs_out/debug_random_505 1024 
./random_pebs ../../traces/memtrace-519.out new_pebs_519.out pebs_out/debug_random_519 1024 
./random_pebs ../../traces/memtrace-557cld.out new_pebs_557cld.out pebs_out/debug_random_557cld 1024 
./random_pebs ../../traces/memtrace-602.out new_pebs_602.out pebs_out/debug_random_602 1024 

./first_seen_pebs ../../traces/memtrace-503.out new_pebs_503.out pebs_out/debug_first_seen_503 1024 
./first_seen_pebs ../../traces/memtrace-505.out new_pebs_505.out pebs_out/debug_first_seen_505 1024 
./first_seen_pebs ../../traces/memtrace-519.out new_pebs_519.out pebs_out/debug_first_seen_519 1024 
./first_seen_pebs ../../traces/memtrace-557cld.out new_pebs_557cld.out pebs_out/debug_first_seen_557cld 1024 
./first_seen_pebs ../../traces/memtrace-602.out new_pebs_602.out pebs_out/debug_first_seen_602 1024 

./dynamic_lfu_pebs ../../traces/memtrace-503.out new_pebs_503.out pebs_out/debug_dlfu_503 1024 
./dynamic_lfu_pebs ../../traces/memtrace-505.out new_pebs_505.out pebs_out/debug_dlfu_505 1024 
./dynamic_lfu_pebs ../../traces/memtrace-519.out new_pebs_519.out pebs_out/debug_dlfu_519 1024 
./dynamic_lfu_pebs ../../traces/memtrace-557cld.out new_pebs_557cld.out pebs_out/debug_dlfu_557cld 1024 
./dynamic_lfu_pebs ../../traces/memtrace-602.out new_pebs_602.out pebs_out/debug_dlfu_602 1024 

./dynamic_lfu_fifo_pebs ../../traces/memtrace-503.out new_pebs_503.out pebs_out/debug_dlfu_fifo_503 1024
./dynamic_lfu_fifo_pebs ../../traces/memtrace-505.out new_pebs_505.out pebs_out/debug_dlfu_fifo_505 1024
./dynamic_lfu_fifo_pebs ../../traces/memtrace-519.out new_pebs_519.out pebs_out/debug_dlfu_fifo_519 1024
./dynamic_lfu_fifo_pebs ../../traces/memtrace-557cld.out new_pebs_557cld.out pebs_out/debug_dlfu_fifo_557cld 1024
./dynamic_lfu_fifo_pebs ../../traces/memtrace-602.out new_pebs_602.out pebs_out/debug_dlfu_fifo_602 1024

./dynamic_lfu_random_pebs ../../traces/memtrace-503.out new_pebs_503.out pebs_out/debug_dlfu_random_503 1024 
./dynamic_lfu_random_pebs ../../traces/memtrace-505.out new_pebs_505.out pebs_out/debug_dlfu_random_505 1024 
./dynamic_lfu_random_pebs ../../traces/memtrace-519.out new_pebs_519.out pebs_out/debug_dlfu_random_519 1024 
./dynamic_lfu_random_pebs ../../traces/memtrace-557cld.out new_pebs_557cld.out pebs_out/debug_dlfu_random_557cld 1024 
./dynamic_lfu_random_pebs ../../traces/memtrace-602.out new_pebs_602.out pebs_out/debug_dlfu_random_602 1024 
