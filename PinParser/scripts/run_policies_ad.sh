./lfu_pebs ../../traces/memtrace-503.out new_ad_503.out 1024 > ad_out/debug_lfu_503
./lfu_pebs ../../traces/memtrace-505.out new_ad_505.out 1024 > ad_out/debug_lfu_505
./lfu_pebs ../../traces/memtrace-519.out new_ad_519.out 1024 > ad_out/debug_lfu_519
./lfu_pebs ../../traces/memtrace-557cld.out new_ad_557cld.out 1024 > ad_out/debug_lfu_557cld
./lfu_pebs ../../traces/memtrace-602.out new_ad_602.out 1024 > ad_out/debug_lfu_602

./random_pebs ../../traces/memtrace-503.out new_ad_503.out ad_out/debug_random_503 1024 
./random_pebs ../../traces/memtrace-505.out new_ad_505.out ad_out/debug_random_505 1024 
./random_pebs ../../traces/memtrace-519.out new_ad_519.out ad_out/debug_random_519 1024 
./random_pebs ../../traces/memtrace-557cld.out new_ad_557cld.out ad_out/debug_random_557cld 1024 
./random_pebs ../../traces/memtrace-602.out new_ad_602.out ad_out/debug_random_602 1024 

./first_seen_pebs ../../traces/memtrace-503.out new_ad_503.out ad_out/debig_fifo_503 1024 
./first_seen_pebs ../../traces/memtrace-505.out new_ad_505.out ad_out/debig_fifo_505 1024
./first_seen_pebs ../../traces/memtrace-557cld.out new_ad_557cld.out ad_out/debig_fifo_557cld 1024
./first_seen_pebs ../../traces/memtrace-519.out new_ad_519.out ad_out/debig_fifo_519 1024
./first_seen_pebs ../../traces/memtrace-602.out new_ad_602.out ad_out/debig_fifo_602 1024 > ad_out/debug_first_seen_602

./dynamic_lfu_ad ../../traces/memtrace-503.out ad_out/dlfu_503 1 1 1024
./dynamic_lfu_ad ../../traces/memtrace-505.out ad_out/dlfu_505 1 1 1024
./dynamic_lfu_ad ../../traces/memtrace-519.out ad_out/dlfu_519 1 1 1024
./dynamic_lfu_ad ../../traces/memtrace-557cld.out ad_out/dlfu_557cld 1 1 1024
./dynamic_lfu_ad ../../traces/memtrace-602.out ad_out/dlfu_602 1 1 1024

./dynamic_lfu_random_ad ../../traces/memtrace-503.out ad_out/dlfu_random_503 1 1 1024
./dynamic_lfu_random_ad ../../traces/memtrace-505.out ad_out/dlfu_random_505 1 1 1024
./dynamic_lfu_random_ad ../../traces/memtrace-519.out ad_out/dlfu_random_519 1 1 1024
./dynamic_lfu_random_ad ../../traces/memtrace-557cld.out ad_out/dlfu_random_557cld 1 1 1024
./dynamic_lfu_random_ad ../../traces/memtrace-602.out ad_out/dlfu_random_602 1 1 1024

./dynamic_lfu_fifo_ad ../../traces/memtrace-503.out ad_out/dlfu_fifo_503 1 1 1024
./dynamic_lfu_fifo_ad ../../traces/memtrace-505.out ad_out/dlfu_fifo_505 1 1 1024
./dynamic_lfu_fifo_ad ../../traces/memtrace-519.out ad_out/dlfu_fifo_519 1 1 1024
./dynamic_lfu_fifo_ad ../../traces/memtrace-557cld.out ad_out/dlfu_fifo_557cld 1 1 1024
./dynamic_lfu_fifo_ad ../../traces/memtrace-602.out ad_out/dlfu_fifo_602 1 1 1024
