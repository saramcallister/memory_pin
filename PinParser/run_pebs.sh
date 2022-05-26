#!/bin/bash

./pebs ../../traces/memtrace-503.out 5 1 63.5 5 > ./503/pebs/pebs_503_5_1 &
./pebs ../../traces/memtrace-505.out 5 1 30 5 > ./505/pebs/pebs_505_5_1 &
./pebs ../../traces/memtrace-519.out 5 1 44 5 > ./519/pebs/pebs_5_1 &
./pebs ../../traces/memtrace-557cld.out 5 1 35 5 > ./557/cld/pebs/pebs_5_1 &
./pebs ../../traces/memtrace-557cpu2006.out 5 1 56.75 5 > ./557/cpu2006/pebs/pebs_5_1 &
./pebs ../../traces/memtrace-557input.out 5 1 36  5 > ./557/input/pebs/pebs_5_1 &
./pebs ../../traces/memtrace-602.out 5 1 102.8 5 > ./602/pebs/pebs_5_1 &
