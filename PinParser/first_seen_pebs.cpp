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

std::vector<PageEntry> getOrderedAccesses(TraceFile &trace) {
    std::unordered_map<uint64_t, uint64_t> pages_to_count;
    std::vector<uint64_t> page_order;
    int num_reads = 0;

//     fprintf(stdout, "starting loop\n");
    EntireEntry ee = trace.readNextEntry();
    while (ee.valid) {
        num_reads ++;

//         fprintf(stdout, "starting for\n");
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

//     fprintf(stdout, "size of page order: %lu\n", page_order.size());

    std::vector<PageEntry> ordered_num_accesses;
//     ordered_num_accesses.reserve(page_order.size());
    for (auto page: page_order) {
        auto it = pages_to_count.find(page);
        ordered_num_accesses.push_back(PageEntry(it->first, it->second));    //add pair to list
    }
//     fprintf(stdout, "size of ordered num accesses: %lu\n", ordered_num_accesses.size());
    return ordered_num_accesses;
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
        if (num_reads % 10001 == 10000) {
            fprintf(stderr, ".");
        }
        ee = trace.readNextEntry();
    }
    std::cout << std::endl;

    return pages_to_count;
}
    
void outputHitsMisses(uint64_t total_pages, 
                std::unordered_map<uint64_t, uint64_t> pin_map,
                std::vector<PageEntry> hotness_ordered_accesses) {
    uint64_t pages_seen = 0;
    uint64_t misses = 0;
    uint64_t hits = 0;

//     fprintf(stdout, "pin_map size: %lu, hotness size: %lu\n", pin_map.size(),
//     hotness_ordered_accesses.size());

    for (auto pe: hotness_ordered_accesses) {
        if (pages_seen >= total_pages) {
            if(pin_map.find(pe.page_num) == pin_map.end()) {
                fprintf(stderr, "THIS SHOULD NEVER HAPPEN!!\n");
            }
            misses += pin_map[pe.page_num];
        } else {
            if(pin_map.find(pe.page_num) == pin_map.end()) {
                fprintf(stderr, "THIS SHOULD NEVER HAPPEN!!\n");
            }
            hits += pin_map[pe.page_num];
        }
        pages_seen++;
    }
    fprintf(outfile, "limit: %ld hits: %ld misses: %ld miss ratio: %f\n", total_pages, hits, misses,
    misses/(double)(hits+misses));
}

int main(int argc, char* argv[]) {

    if (argc < 5) {
        std::cerr << "./first_seen pin_trace hotness_trace outputfile numpages..numpages" << std::endl;
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
    
//     auto pin_ordered = getOrderedAccesses(pin_trace);
//     fprintf(stdout, "pin trace function\n");
    auto pin_map = getAccesses(pin_trace);
//     fprintf(stdout, "hotness function\n");
    auto hotness_ordered = getOrderedAccesses(hotness_trace);

    int arg_num = 4;
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        outputHitsMisses(page_limit, pin_map, hotness_ordered);
        arg_num++;
    }
}
