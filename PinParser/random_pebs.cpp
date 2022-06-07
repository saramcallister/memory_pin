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

TraceFile pin_trace;
TraceFile hotness_trace;
FILE* outfile;

static unsigned int gseed=123456789;
inline int fastrand() {
    gseed = (214013 * gseed + 2531011);
    return (gseed >> 16) & 0x7fff;
}

std::unordered_map<uint64_t, uint64_t> getAccesses(TraceFile &trace) {
    std::unordered_map<uint64_t, uint64_t> pages_to_count;
    int num_reads = 0;

    EntireEntry ee = trace.readNextEntry();
    while (ee.valid) {
        num_reads ++;

        for (auto pe: ee.pe_list) {
            auto it = pages_to_count.find(pe.page_num);
            if (it == pages_to_count.end()) {
                pages_to_count[pe.page_num] = std::min<uint64_t>(pe.accesses, 64);
            } else {
                it->second += std::min<uint64_t>(pe.accesses, 64);
            }
        }
        if (num_reads % 1001 == 1000) {
            fprintf(stderr, ".");
        }
        ee = trace.readNextEntry();
    }
    std::cout << std::endl;

    return pages_to_count;
}
    
void outputHitsMisses(uint64_t local_pages, 
            std::unordered_map<uint64_t, uint64_t> pin_accesses, 
            std::unordered_map<uint64_t, uint64_t> hotness_accesses) {
    uint64_t allocated_local = 0;
    uint64_t misses = 0;
    uint64_t hits = 0;

    uint64_t all_pages = pin_accesses.size();
    for (auto entry: pin_accesses) {
        bool try_local = fastrand() % all_pages <= local_pages;
        if (try_local && allocated_local < local_pages &&
            (hotness_accesses.find(entry.first) != hotness_accesses.end())) {
            hits += entry.second;
            allocated_local++;
        } else {
            misses += entry.second;
        }
    }
    fprintf(outfile, "%ld %ld %ld %f\n", local_pages, hits, misses, ( (double)
    misses)/(hits+misses));
}

int main(int argc, char* argv[]) {

    if (argc < 5) {
        std::cerr << "./random ground_truth_trace input_trace outputfile numpages..numpages" << std::endl;
        return 1;
    }

    pin_trace.setTraceFile(argv[1]);
    hotness_trace.setTraceFile(argv[2]);

    outfile = fopen(argv[3], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[3] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    auto pin_accesses = getAccesses(pin_trace);
    auto hotness_accesses = getAccesses(hotness_trace);

    int arg_num = 4;
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        outputHitsMisses(page_limit, pin_accesses, hotness_accesses);
        arg_num++;
    }
}
