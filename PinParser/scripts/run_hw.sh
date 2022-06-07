#!/bin/bash

./hw_hotness ../../traces/memtrace-503.out 50 63.5 5 > ./503/hw/hw_503_5_5 &
./hw_hotness ../../traces/memtrace-505.out 50 30 5 > ./505/hw/hw_505_5_5 &
./hw_hotness ../../traces/memtrace-519.out 50 44 5 > ./519/hw/hw_5_5 &
./hw_hotness ../../traces/memtrace-557cld.out 50 35 5 > ./557/cld/hw/hw_5_5 &
./hw_hotness ../../traces/memtrace-602.out 50 102.8 5 > ./602/hw/hw_5_5 &
