#!/bin/bash

myFiles=("503" "505" "519" "557cld" "602")
pages=("55812" "43002" "26197" "50987" "491174")

for i in {0..4};    #lfu_pebs
do 
    ./lfu_pebs ../../traces/memtrace-${myFiles[$i]}.out new_pebs_${myFiles[$i]}.out ${pages[$i]}> pebs_out/lfu_${myFiles[$i]}_25
done

for i in {0..4};    #random_pebs
do 
    ./random_pebs ../../traces/memtrace-${myFiles[$i]}.out new_pebs_${myFiles[$i]}.out pebs_out/random_${myFiles[$i]}_25 ${pages[$i]}
done

for i in {0..4};    #first_seen_pebs
do 
    ./first_seen_pebs ../../traces/memtrace-${myFiles[$i]}.out new_pebs_${myFiles[$i]}.out pebs_out/fifo_${myFiles[$i]}_25 ${pages[$i]}
done

for i in {0..4}; #dynamin_lfu_pebs
do 
    ./dynamic_lfu_pebs ../../traces/memtrace-${myFiles[$i]}.out new_pebs_${myFiles[$i]}.out pebs_out/dlfu_${myFiles[$i]}_25 ${pages[$i]}
done

for i in {0..4}; #dynamin_lfu_fifo_pebs
do 
    ./dynamic_lfu_fifo_pebs ../../traces/memtrace-${myFiles[$i]}.out new_pebs_${myFiles[$i]}.out pebs_out/dlfu_fifo_${myFiles[$i]}_25 ${pages[$i]}
done

for i in {0..4}; #dynamin_lfu_random_pebs
do 
    ./dynamic_lfu_random_pebs ../../traces/memtrace-${myFiles[$i]}.out new_pebs_${myFiles[$i]}.out pebs_out/dlfu_random_${myFiles[$i]}_25 ${pages[$i]}
done
