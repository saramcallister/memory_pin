#!/bin/bash

# ./pebs ../../traces/memtrace-503.out 5 63.5 5 > ./503/pebs/pebs_503_5_5 &
# ./pebs ../../traces/memtrace-505.out 5 30 5 > ./505/pebs/pebs_505_5_5 &
# ./pebs ../../traces/memtrace-519.out 5 44 5 > ./519/pebs/pebs_5_5 &
# ./pebs ../../traces/memtrace-557cld.out 5 35 5 > ./557/cld/pebs/pebs_5_5 &
# ./pebs ../../traces/memtrace-557cpu2006.out 5 56.75 5 > ./557/cpu2006/pebs/pebs_5_5 &
# ./pebs ../../traces/memtrace-557input.out 5 36  5 > ./557/input/pebs/pebs_5_5 &
# ./pebs ../../traces/memtrace-602.out 5 102.8 5 > ./602/pebs/pebs_5_5 &

./pebs ../../traces/memtrace-503.out 50 63.5 5 > ./503/pebs/pebs_503_5_5 &
./pebs ../../traces/memtrace-505.out 50 30 5 > ./505/pebs/pebs_505_5_5 &
./pebs ../../traces/memtrace-519.out 50 44 5 > ./519/pebs/pebs_5_5 &
./pebs ../../traces/memtrace-557cld.out 50 35 5 > ./557/cld/pebs/pebs_5_5 &
./pebs ../../traces/memtrace-602.out 50 102.8 5 > ./602/pebs/pebs_5_5 &
