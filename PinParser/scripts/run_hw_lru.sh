#!/bin/bash
pages=("55812" "43002" "26197" "50987" "491174")
scale=("63.5" "30" "44" "35" "102.8")
pages=("111625" "86004" "52394" "101975" "982349")

 ./lru_hw ../../traces/small-503.out hw/lru_503_small 63.5 55812 111625 &
./lru_hw ../../traces/small-505.out hw/lru_505_small 30 3583 7166 &
./lru_hw ../../traces/small-519.out hw/lru_519_small 44 5240 10480 &
./lru_hw ../../traces/small-557.out hw/lru_557_small 35 5665 11330 &
./lru_hw ../../traces/small-602.out hw/lru_602_small 102.8 61396 122793  &
