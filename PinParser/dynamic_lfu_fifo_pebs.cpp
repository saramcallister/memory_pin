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

TraceFile pinTrace;
TraceFile hotnessTrace;
FILE* outfile;

static unsigned int gseed=123456789;
inline int fastrand() {
    gseed = (214013 * gseed + 2531011);
    return (gseed >> 16) & 0x7fff;
}

std::vector<PageEntry> traceToMap(TraceFile &trace, int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
//     std::unordered_map<uint64_t, uint64_t> page_to_count;
    std::vector<PageEntry> trace_map;
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
//             auto it = page_to_count.find(pe.page_num);
//             if (it == page_to_count.end()) {
//                 page_to_count[pe.page_num] = std::min<uint64_t>(pe.accesses, 64);
//             } else {
//                 it->second += std::min<uint64_t>(pe.accesses, 64);
//             }
            trace_map.push_back(pe);
        }
        if (num_reads % 10001 == 10000) {
            fprintf(stderr, ".");
        }
    }
    std::cout << std::endl;
    return trace_map;
}


std::vector<uint64_t> getStartingFIFO(std::vector<PageEntry> map, uint64_t total_pages) {
    std::vector<uint64_t> pages;
    for(auto pe:map) {
        if(pages.size() >= total_pages) {
            break;
        }
        pages.push_back(pe.page_num);
    }

    return pages;
}

void dynamicLFU(std::vector<uint64_t> starting_pages, uint64_t total_pages, int
timing, int scale) {
    EntireEntry pin_ee = pinTrace.readNextEntry();
    EntireEntry hotness_ee = hotnessTrace.readNextEntry();

    std::unordered_set<uint64_t> current_pages;
    std::copy(starting_pages.begin(), starting_pages.end(), inserter(current_pages, current_pages.begin()));

    uint64_t misses = 0;
    uint64_t hits = 0;

    double start_time = pin_ee.eh.time * 1000; // seconds to ms
    std::unordered_map<uint64_t, uint64_t> hits_in_current_time;

    while (pin_ee.valid) {

        if (pin_ee.eh.time * 1000 - start_time > scale*timing) {
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
            start_time = pin_ee.eh.time * 1000;
        
        }

        for (auto pe: pin_ee.pe_list) {
            if (current_pages.find(pe.page_num) == current_pages.end()) {
                misses += pe.accesses;
            }  else {
                hits += pe.accesses;
            }
        }

        for(auto pe: hotness_ee.pe_list) {
            auto hit_tracking = hits_in_current_time.find(pe.page_num);
            if (hit_tracking == hits_in_current_time.end()) {
                hits_in_current_time[pe.page_num] = pe.accesses;
            } else {
                hit_tracking->second += pe.accesses;
            }
        }

        pin_ee = pinTrace.readNextEntry();
        hotness_ee = hotnessTrace.readNextEntry();
    }

    fprintf(outfile, "%ld %d %ld %ld %f\n", total_pages, timing, hits, misses, 
        ((double)misses)/(double)(hits+misses));

} 

int main(int argc, char* argv[]) {

    if (argc < 4) {
        std::cerr << "./lfu pin_trace hotness_trace outputfile scale numpages..numpages" << std::endl;
        return 1;
    }

    pinTrace.setTraceFile(argv[1]);
    hotnessTrace.setTraceFile(argv[2]);

    auto initial_mapping = traceToMap(hotnessTrace);

    outfile = fopen(argv[3], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[3] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }

    double scale = std::stof(argv[4]);
    int arg_num = 5;
    std::vector<int> page_mvmt_speeds{10, 100, 1000, 5000, 10000, 100000}; // 1 page mvmt per x ms
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
//         std::vector<uint64_t> starting_pages = getStartingPages(initial_mapping, page_limit);
        std::vector<uint64_t> starting_pages =
        getStartingFIFO(initial_mapping, page_limit);
        for (const auto timing : page_mvmt_speeds) {
//             trace = TraceFile();
//             trace.setTraceFile(argv[1]); // reset to run through again
            pinTrace = TraceFile();
            pinTrace.setTraceFile(argv[1]);
            hotnessTrace = TraceFile();
            hotnessTrace.setTraceFile(argv[2]);

            dynamicLFU(starting_pages, page_limit, timing, scale);
        }
        arg_num++;
    }
}
