#include <algorithm>
#include <cstdint>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parser.hpp"

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "./working_set_size tracefile outputfile" << std::endl;
        return 1;
    }

    TraceFile trace;
    trace.setTraceFile(argv[1]);

    FILE* outfile = fopen(argv[2], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[2] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }


    std::unordered_map<uint64_t, uint64_t> size_to_count;
    EntireEntry ee = trace.readNextEntry();
    while (ee.valid) {
        uint64_t size = ee.pe_list.size();
        auto it = size_to_count.find(size);
        if (it == size_to_count.end()) {
            size_to_count[size] = 1;
        } else {
            it->second++;
        }
        ee = trace.readNextEntry();
    }

    for (auto [size, count]: size_to_count) {
        fprintf(outfile, "%ld %ld\n", size, count);
    }

}
