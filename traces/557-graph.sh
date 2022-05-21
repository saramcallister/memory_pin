total_pages=203950
# num_pages=$(echo "0.1 * $total_pages" | bc)
# ../PinParser/lfu memtrace-557cld.out $num_pages > out_557_1
# num_pages=$(echo "0.2 * $total_pages" | bc)
# ../PinParser/lfu memtrace-557cld.out $num_pages > out_557_2
# num_pages=$(echo "0.3 * $total_pages" | bc)
# ../PinParser/lfu memtrace-557cld.out $num_pages > out_557_3
# num_pages=$(echo "0.4 * $total_pages" | bc)
# ../PinParser/lfu memtrace-557cld.out $num_pages > out_557_4
# num_pages=$(echo "0.5 * $total_pages" | bc)
# ../PinParser/lfu memtrace-557cld.out $num_pages > out_557_5
# num_pages=$(echo "0.6 * $total_pages" | bc)
# ../PinParser/lfu memtrace-557cld.out $num_pages > out_557_6
# num_pages=$(echo "0.7 * $total_pages" | bc)
# ../PinParser/lfu memtrace-557cld.out $num_pages > out_557_7
# num_pages=$(echo "0.8 * $total_pages" | bc)
# ../PinParser/lfu memtrace-557cld.out $num_pages > out_557_8
# num_pages=$(echo "0.9 * $total_pages" | bc)
# ../PinParser/lfu memtrace-557cld.out $num_pages > out_557_9
# num_pages=$(echo "0 * $total_pages" | bc)
# ../PinParser/lfu memtrace-557cld.out $num_pages > out_557_0

# num_pages=$(echo "0.1 * $total_pages" | bc)
# ../PinParser/random memtrace-557cld.out out_557_1_random $num_pages 
# num_pages=$(echo "0.2 * $total_pages" | bc)
# ../PinParser/random memtrace-557cld.out out_557_2_random $num_pages 
# num_pages=$(echo "0.3 * $total_pages" | bc)
# ../PinParser/random memtrace-557cld.out out_557_3_random $num_pages 
# num_pages=$(echo "0.4 * $total_pages" | bc)
# ../PinParser/random memtrace-557cld.out out_557_4_random $num_pages 
# num_pages=$(echo "0.5 * $total_pages" | bc)
# ../PinParser/random memtrace-557cld.out out_557_5_random $num_pages 
# num_pages=$(echo "0.6 * $total_pages" | bc)
# ../PinParser/random memtrace-557cld.out out_557_6_random $num_pages 
# num_pages=$(echo "0.7 * $total_pages" | bc)
# ../PinParser/random memtrace-557cld.out out_557_7_random $num_pages 
# num_pages=$(echo "0.8 * $total_pages" | bc)
# ../PinParser/random memtrace-557cld.out out_557_8_random $num_pages 
# num_pages=$(echo "0.9 * $total_pages" | bc)
# ../PinParser/random memtrace-557cld.out out_557_9_random $num_pages 
# num_pages=$(echo "0 * $total_pages" | bc)
# ../PinParser/random memtrace-557cld.out out_557_0_random $num_pages 

../PinParser/lfu memtrace-557cld.out 50000 > out_557_lfu_1
../PinParser/lfu memtrace-557cld.out 75000 > out_557_lfu_2
../PinParser/lfu memtrace-557cld.out 100000 > out_557_lfu_3
../PinParser/lfu memtrace-557cld.out 150000 > out_557_lfu_4
../PinParser/lfu memtrace-557cld.out 191232 > out_557_lfu_5
../PinParser/lfu memtrace-557cld.out 200000 > out_557_lfu_6
../PinParser/lfu memtrace-557cld.out 400000 > out_557_lfu_7

