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

TraceFile trace;
FILE* outfile;

static unsigned int gseed=123456789;
inline int fastrand() {
    gseed = (214013 * gseed + 2531011);
    return (gseed >> 16) & 0x7fff;
}

std::unordered_map<uint64_t, uint64_t> getAccesses() {
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
    
void outputHitsMisses(uint64_t local_pages, std::unordered_map<uint64_t, uint64_t> accesses) {
    uint64_t allocated_local = 0;
    uint64_t misses = 0;
    uint64_t hits = 0;

    uint64_t all_pages = accesses.size();
    for (auto entry: accesses) {
        bool try_local = fastrand() % all_pages <= local_pages;
        if (try_local && allocated_local < local_pages) {
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

    if (argc < 4) {
        std::cerr << "./first_seen tracefile outputfile numpages..numpages" << std::endl;
        return 1;
    }

    trace.setTraceFile(argv[1]);

    outfile = fopen(argv[2], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[2] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    auto accesses = getAccesses();

    int arg_num = 3;
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        outputHitsMisses(page_limit, accesses);
        arg_num++;
    }
}
