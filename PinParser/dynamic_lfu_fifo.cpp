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

std::vector<uint64_t> getStartingFIFO(uint64_t total_pages, int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
//     std::unordered_map<uint64_t, uint64_t> page_to_count;
//     std::vector<PageEntry> trace_map;

    std::vector<uint64_t> pages;
    int64_t seen_instr = 0;
    int num_reads = 0;
    int64_t start_instr = 0;
    bool first = true;
    while (instr_limit > seen_instr - start_instr) {
        num_reads ++;
        EntireEntry ee = trace.readNextEntry();
        if (!ee.valid) {
            break;
        }

        if (first) {
            start_instr = ee.eh.instructions;
            first = false;
        }
        seen_instr += ee.eh.instructions;

        for (auto pe: ee.pe_list) {
//             trace_map.push_back(pe);
            if(pages.size() >= total_pages) {
                return pages;
            }
            pages.push_back(pe.page_num);
        }
        if (num_reads % 10001 == 10000) {
            fprintf(stderr, ".");
        }
    }
    std::cout << std::endl;
    return pages;
}


void dynamicLFU(std::vector<uint64_t> starting_pages, uint64_t total_pages, int
timing, int scale) {
    EntireEntry ee = trace.readNextEntry();

    std::unordered_set<uint64_t> current_pages;
    std::copy(starting_pages.begin(), starting_pages.end(), inserter(current_pages, current_pages.begin()));

    uint64_t misses = 0;
    uint64_t hits = 0;

    double start_time = ee.eh.time * 1000; // seconds to ms
    std::unordered_map<uint64_t, uint64_t> hits_in_current_time;

    while (ee.valid) {

        if (ee.eh.time * 1000 - start_time > scale*timing) {
            uint64_t least_accesses = -1;
            uint64_t victim_page;
            for (auto& page: current_pages) {
                auto it = hits_in_current_time.find(page);
                if (it == hits_in_current_time.end()) {
                    victim_page = page; 
                    break;
                } else if (it->second < least_accesses) {
                    victim_page = page;
                    least_accesses = it->second;
                }
            }
            current_pages.erase(victim_page);

            uint64_t most_accesses = 0;
            uint64_t promoted_page;
            for (auto &[page, accesses]: hits_in_current_time) {
                if (accesses > most_accesses) {
                    most_accesses = accesses;
                    promoted_page = page;
                }
            }
            current_pages.insert(promoted_page);
            hits_in_current_time.clear();
            start_time = ee.eh.time * 1000;
        
        }

        for (auto pe: ee.pe_list) {
            if (current_pages.find(pe.page_num) == current_pages.end()) {
                misses += pe.accesses;
            }  else {
                hits += pe.accesses;
            }

            auto hit_tracking = hits_in_current_time.find(pe.page_num);
            if (hit_tracking == hits_in_current_time.end()) {
                hits_in_current_time[pe.page_num] = pe.accesses;
            } else {
                hit_tracking->second += pe.accesses;
            }
        }

        ee = trace.readNextEntry();
    }

    fprintf(outfile, "%ld %d %ld %ld %f\n", total_pages, timing, hits, misses,
    ((double)misses/(double(hits+misses))));

} 

int main(int argc, char* argv[]) {

    if (argc < 4) {
        std::cerr << "./lfu tracefile outputfile scale numpages..numpages" << std::endl;
        return 1;
    }

    trace.setTraceFile(argv[1]);

//     auto initial_mapping = aggregatePages();

    outfile = fopen(argv[2], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[2] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }

    double scale = std::stof(argv[3]);

    int arg_num = 4;
    std::vector<int> page_mvmt_speeds{10, 100, 1000, 5000, 10000, 100000}; // 1 page mvmt per x ms
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        std::vector<uint64_t> starting_pages = getStartingFIFO(page_limit);
        for (const auto timing : page_mvmt_speeds) {
            trace = TraceFile();
            trace.setTraceFile(argv[1]); // reset to run through again

            dynamicLFU(starting_pages, page_limit, timing,scale);
        }
        arg_num++;
    }
}
