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

std::vector<uint64_t> getOrderedAccesses() {
    std::unordered_map<uint64_t, uint64_t> pages_to_count;
    std::vector<uint64_t> page_order;
    int num_reads = 0;

    EntireEntry ee = trace.readNextEntry();
    while (ee.valid) {
        num_reads ++;

        for (auto pe: ee.pe_list) {
            auto it = pages_to_count.find(pe.page_num);
            if (it == pages_to_count.end()) {
                pages_to_count[pe.page_num] = std::min<uint64_t>(pe.accesses, 64);
                page_order.push_back(pe.page_num);
            } else {
                it->second += std::min<uint64_t>(pe.accesses, 64);
            }
        }
        if (num_reads % 10001 == 10000) {
            fprintf(stderr, ".");
        }
        ee = trace.readNextEntry();
    }
    std::cout << std::endl;

    std::vector<uint64_t> ordered_num_accesses;
    ordered_num_accesses.reserve(page_order.size());
    for (auto page: page_order) {
        auto it = pages_to_count.find(page);
        ordered_num_accesses.push_back(it->second);
    }
    return ordered_num_accesses;
}
    
void outputHitsMisses(uint64_t total_pages, std::vector<uint64_t> ordered_accesses) {
    uint64_t pages_seen = 0;
    uint64_t misses = 0;
    uint64_t hits = 0;
    for (auto accesses: ordered_accesses) {
        if (pages_seen >= total_pages) {
            misses += accesses;
        } else {
            hits += accesses;
        }
        pages_seen++;
    }
    fprintf(outfile, 
        "limit: %ld hits: %ld misses: %ld miss ratio: %f\n", total_pages, hits, misses,
        (double)(misses)/(double)(hits+misses));
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
    
    auto ordered = getOrderedAccesses();

    int arg_num = 3;
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        outputHitsMisses(page_limit, ordered);
        arg_num++;
    }
}
