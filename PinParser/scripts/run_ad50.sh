#!/bin/bash

myFiles=("503" "505" "519" "557cld" "602")
pages=("111625" "86004" "52394" "101975" "982349")

for i in {0..4};    #lfu_pebs
do 
    ./lfu_pebs ../../traces/memtrace-${myFiles[$i]}.out new_ad_${myFiles[$i]}.out ${pages[$i]}> ad_out/lfu_${myFiles[$i]}_50
done

for i in {0..4};    #random_pebs
do 
    ./random_pebs ../../traces/memtrace-${myFiles[$i]}.out new_ad_${myFiles[$i]}.out ad_out/random_${myFiles[$i]}_50 ${pages[$i]}
done

for i in {0..4};    #first_seen_pebs
do 
    ./first_seen_pebs ../../traces/memtrace-${myFiles[$i]}.out new_ad_${myFiles[$i]}.out ad_out/fifo_${myFiles[$i]}_50 ${pages[$i]}
done

for i in {0..4}; #dynamin_lfu_pebs
do 
    ./dynamic_lfu_pebs ../../traces/memtrace-${myFiles[$i]}.out ad_out/dlfu_${myFiles[$i]}_50 1 1 ${pages[$i]}
done

for i in {0..4}; #dynamin_lfu_fifo_pebs
do 
    ./dynamic_lfu_fifo_pebs ../../traces/memtrace-${myFiles[$i]}.out ad_out/dlfu_fifo_${myFiles[$i]}_50 1 1 ${pages[$i]}
done

for i in {0..4}; #dynamin_lfu_random_pebs
do 
    ./dynamic_lfu_random_pebs ../../traces/memtrace-${myFiles[$i]}.out ad_out/dlfu_random_${myFiles[$i]}_50 1 1 ${pages[$i]}
done

