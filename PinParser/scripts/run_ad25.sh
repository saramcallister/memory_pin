#!/bin/bash

myFiles=("503" "505" "519" "557cld" "602")
pages=("55812" "43002" "26197" "50987" "491174")
pages2=("111625" "86004" "52394" "101975" "982349")
scale=("63.5" "30" "44" "35" "102.8")

# for i in {0..4};    #lfu_pebs
# do 
#     ./lfu_pebs ../../traces/memtrace-${myFiles[$i]}.out traces/new_ad_${myFiles[$i]}.out ${pages[$i]}> ad_out/lfu_${myFiles[$i]}_25
# done
# 
# for i in {0..4};    #random_pebs
# do 
#     ./random_pebs ../../traces/memtrace-${myFiles[$i]}.out traces/new_ad_${myFiles[$i]}.out ad_out/random_${myFiles[$i]}_25 ${pages[$i]}
# done
# 
# for i in {0..4};    #first_seen_pebs
# do 
#     ./first_seen_pebs ../../traces/memtrace-${myFiles[$i]}.out traces/new_ad_${myFiles[$i]}.out ad_out/fifo_${myFiles[$i]}_25 ${pages[$i]}
# done

# for i in {0..4}; #dynamin_lfu_pebs
# do 
#     ./dynamic_lfu_ad ../../traces/memtrace-${myFiles[$i]}.out ad_out/dlfu_${myFiles[$i]}_25 1 1 ${scale[$i]} ${pages[$i]} 
# done

# for i in {0..4}; #dynamin_lfu_fifo_pebs
# do 
#     ./dynamic_lfu_fifo_ad ../../traces/memtrace-${myFiles[$i]}.out ad_out/dlfu_fifo_${myFiles[$i]}_25 1 1 ${scale[$i]}  ${pages[$i]} 
# done
# 
# for i in {0..4}; #dynamin_lfu_random_pebs
# do 
#     ./dynamic_lfu_random_ad ../../traces/memtrace-${myFiles[$i]}.out ad_out/dlfu_random_${myFiles[$i]}_25 1 1 ${scale[$i]}  ${pages[$i]}
# done

# ./dynamic_lfu_ad ../../traces/memtrace-503.out ad_out/dlfu_503_25 1 5 63.5 55812 &
# ./dynamic_lfu_ad ../../traces/memtrace-505.out ad_out/dlfu_505_25 1 5 30 43002 & 
# ./dynamic_lfu_ad ../../traces/memtrace-519.out ad_out/dlfu_519_25 1 5 44 26197 &
# ./dynamic_lfu_ad ../../traces/memtrace-557cld.out ad_out/dlfu_557cld_25 1 5 36 50987 & 
# ./dynamic_lfu_ad ../../traces/memtrace-602.out ad_out/dlfu_602_25 1 5 102.8 49117 &
# 
# ./dynamic_lfu_fifo_ad ../../traces/memtrace-503.out ad_out/dlfu_fifo_503_25 1 5 63.5 55812 &
# ./dynamic_lfu_fifo_ad ../../traces/memtrace-505.out ad_out/dlfu_fifo_505_25 1 5 30 43002 &
# ./dynamic_lfu_fifo_ad ../../traces/memtrace-519.out ad_out/dlfu_fifo_519_25 1 5 44 26197 &
# ./dynamic_lfu_fifo_ad ../../traces/memtrace-557cld.out ad_out/dlfu_fifo_557cld_25 1 5 36 50987 &
# ./dynamic_lfu_fifo_ad ../../traces/memtrace-602.out ad_out/dlfu_fifo_602_25 1 5 102.8 49117 &

for i in {0..4}; #lru
do 
    ./lru_pebs ../../traces/memtrace-${myFiles[$i]}.out traces/new_ad_${myFiles[$i]}.out ad_out/lru_${myFiles[$i]} ${scale[$i]} ${pages[$i]} ${pages2[$i]}

done
