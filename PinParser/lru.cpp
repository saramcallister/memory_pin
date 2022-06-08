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

std::unordered_map<uint64_t, uint64_t> aggregatePages() {
    std::unordered_map<uint64_t, uint64_t> initial;

    EntireEntry ee = trace.readNextEntry();
    uint64_t num_reads = 0;

    while(ee.valid) {
        num_reads++;
        
        for(auto pe: ee.pe_list) {
            auto it = initial.find(pe.page_num);

            if(it == initial.end()) {
                initial[pe.page_num] = std::min<uint64_t>(pe.accesses, 64);
            } else {
                it->second += std::min<uint64_t>(pe.accesses,64);
            }
        }
        if (num_reads % 10001 == 10000) {
            fprintf(stderr, ".");
        }

        ee = trace.readNextEntry();
    }

    return initial;
}


std::vector<uint64_t> getStartingPages(std::unordered_map<uint64_t, uint64_t>
initial, uint64_t total_pages) {

    std::priority_queue <PageEntry, std::vector<PageEntry>, PageEntryComparator> pq;
    for (auto & [ page_num, accesses ] : initial) {
        pq.push(PageEntry(page_num, accesses));
    }

    std::vector<uint64_t> pages;
    pages.reserve(total_pages);
    for (uint64_t i = 0; i < total_pages; i++) {
        PageEntry pe = pq.top();
        pages.push_back(pe.page_num);
        pq.pop();
    }
    return pages;
}

void lru(std::vector<uint64_t> starting_pages, uint64_t total_pages, int
timing, double scale) {
    EntireEntry ee = trace.readNextEntry();

    std::unordered_set<uint64_t> current_pages;
    std::copy(starting_pages.begin(), starting_pages.end(), inserter(current_pages, current_pages.begin()));

    uint64_t misses = 0;
    uint64_t hits = 0;

    uint64_t pages_moved = 0;

    double start_time = ee.eh.time * 1000; // seconds to ms
    std::unordered_map<uint64_t, double> hits_to_time;

    while (ee.valid) {

        if (ee.eh.time * 1000 - start_time > scale*timing) {
            double min_time = ee.eh.time;
            uint64_t victim_page;
            for (auto& page: current_pages) {
                auto it = hits_to_time.find(page);
                if (it == hits_to_time.end()) {
                    victim_page = page; 
                    break;
                } else if (it->second< min_time) {
                    victim_page = page;
                    min_time = it->second;
                }
            }
            current_pages.erase(victim_page);

            double max_time = -1;
            uint64_t promoted_page;
            for (auto &[page, time]: hits_to_time) {
                if ( time > max_time) {
                    max_time = time;
                    promoted_page = page;
                }
            }
            current_pages.insert(promoted_page);
            pages_moved++;
            hits_to_time.clear();
            start_time = ee.eh.time * 1000;
        
        }

        for (auto pe: ee.pe_list) {
            if (current_pages.find(pe.page_num) == current_pages.end()) {
                misses += pe.accesses;
            }  else {
                hits += pe.accesses;
            }

            auto hit_tracking = hits_to_time.find(pe.page_num);
            if (hit_tracking == hits_to_time.end()) {
                hits_to_time[pe.page_num] = ee.eh.time;
            } else {
                hit_tracking->second = ee.eh.time;
            }
        }

        ee = trace.readNextEntry();
    }

    fprintf(outfile, "%ld %d %lu %ld %ld %f\n", total_pages, timing,
    pages_moved, hits, misses,
    ((double)misses/(double(hits+misses))));

} 

int main(int argc, char* argv[]) {

    if (argc < 5) {
        std::cerr << "./lru tracefile outputfile time_scale numpages..numpages" << std::endl;
        return 1;
    }

    trace.setTraceFile(argv[1]);

    auto initial_mapping = aggregatePages();

    outfile = fopen(argv[2], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[2] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }

    double scale = std::stof(argv[3]);

    int arg_num = 4;
//     std::vector<int> page_mvmt_speeds{10, 100, 1000, 5000, 10000, 100000}; // 1 page mvmt per x ms
    std::vector<int> page_mvmt_speeds{10}; // 1 page mvmt per x ms
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        std::vector<uint64_t> starting_pages = getStartingPages(initial_mapping, page_limit);
        for (const auto timing : page_mvmt_speeds) {
            trace = TraceFile();
            trace.setTraceFile(argv[1]); // reset to run through again

            lru(starting_pages, page_limit, timing, scale);
        }
        arg_num++;
    }
}
