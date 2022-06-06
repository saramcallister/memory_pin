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
#include <vector>

#include "parser.hpp"

TraceFile trace;
FILE* outfile;

std::unordered_map<uint64_t, uint64_t> aggregatePages(std::vector<uint64_t>& ordered) {
    std::unordered_map<uint64_t, uint64_t> page_to_count;
    int num_reads = 0;
    
    EntireEntry ee = trace.readNextEntry();
    while (ee.valid) {
        num_reads ++;

        for (auto pe: ee.pe_list) {
            auto it = page_to_count.find(pe.page_num);
            if (it == page_to_count.end()) {
                page_to_count[pe.page_num] = std::min<uint64_t>(pe.accesses, 64);
                ordered.push_back(pe.page_num);
            } else {
                it->second += std::min<uint64_t>(pe.accesses, 64);
            }
        }
        if (num_reads % 1001 == 1000) {
            std::cerr << '.';
        }
        ee = trace.readNextEntry();
    }
    std::cerr << std::endl;
    return page_to_count;
}

std::unordered_map<uint64_t, uint64_t> getSegments(std::unordered_map<uint64_t, uint64_t> ptc,
        std::vector<uint64_t> page_order, uint64_t pages_per_segment) {
    std::unordered_map<uint64_t, uint64_t> segment_to_accesses;
    uint64_t segment_num = 0;
    uint64_t pages_allocated = 0;
    uint64_t segment_accesses = 0;
    for (uint64_t page : page_order) {
        pages_allocated++;
        auto it = ptc.find(page);
        segment_accesses += it->second;

        if (pages_allocated >= pages_per_segment) {
            segment_to_accesses[segment_num] = segment_accesses;
            segment_accesses = 0;
            pages_allocated = 0;
            segment_num++;
        }

    }
    segment_to_accesses[segment_num] = segment_accesses;
    return segment_to_accesses;
}


void outputHitsMisses(std::unordered_map<uint64_t, uint64_t> segments_to_accesses, 
        uint64_t total_segments, uint64_t pages_per_segment) {
    std::priority_queue <PageEntry, std::vector<PageEntry>, PageEntryComparator> pq;
    for (auto & [ segment, accesses ] : segments_to_accesses) {
        pq.push(PageEntry(segment, accesses));
    }

    uint64_t misses = 0;
    uint64_t hits = 0;
    uint64_t pages_taken = 0;
    while (pq.empty() == false) {
        PageEntry pe = pq.top();
        if (pages_taken < total_segments) {
            hits += pe.accesses;
        } else {
            misses += pe.accesses;
        }
        pages_taken++;
        pq.pop();
    }

    fprintf(stdout, "Hits: %ld Misses: %ld Miss Ratio: %f\n", hits, misses, misses / (double) (hits + misses));
    fprintf(outfile, "%ld %ld %ld %ld\n", total_segments * pages_per_segment, pages_per_segment, hits, misses);
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "./lfu tracefile numpages..numpages" << std::endl;
        return 1;
    }

    trace.setTraceFile(argv[1]);
    
    outfile = fopen(argv[2], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[2] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    std::vector<uint64_t> page_order;
    auto mapping = aggregatePages(page_order);
    
    int arg_num = 3;
    std::vector<uint64_t> segment_sizes{1, 4, 16, 512, 2048};
    std::vector<uint64_t> local_pages;
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        local_pages.push_back(page_limit);
        arg_num++;
    }

    for (auto pages_per_segment: segment_sizes) {
        auto seg_to_accesses = getSegments(mapping, page_order, pages_per_segment);
        for (auto num_pages: local_pages) {
            outputHitsMisses(seg_to_accesses, num_pages / pages_per_segment, pages_per_segment);
        }
    }
}

