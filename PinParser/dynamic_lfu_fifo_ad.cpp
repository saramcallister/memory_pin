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
// TraceFile hotnessTrace;
FILE* outfile;

std::vector<uint64_t> getStartingFIFO(TraceFile &trace, uint64_t total_pages, int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
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
timing, int num_bits, uint64_t sample_time, double scale) {
    EntireEntry pin_ee = pinTrace.readNextEntry();

    std::unordered_set<uint64_t> current_pages;
    std::unordered_map<uint64_t, uint64_t> accessed_pages;
    std::unordered_map<uint64_t, uint64_t> page_to_ad_bits;

    uint64_t last_sample_time = pin_ee.eh.time;

    std::copy(starting_pages.begin(), starting_pages.end(), inserter(current_pages, current_pages.begin()));

    uint64_t misses = 0;
    uint64_t hits = 0;

    double start_time = pin_ee.eh.time * 1000; // seconds to ms

    while (pin_ee.valid) {

        for (auto pe: pin_ee.pe_list) {
            if (current_pages.find(pe.page_num) == current_pages.end()) {
                misses += pe.accesses;
            }  else {
                hits += pe.accesses;
            }

            auto accessed_page = accessed_pages.find(pe.page_num);
            if (accessed_page == accessed_pages.end()) {
                accessed_pages[pe.page_num] = 1;
            }
        }

        if((pin_ee.eh.time - last_sample_time) >= scale*sample_time) { // this stuff is in s
            for (auto entry : accessed_pages) {
                auto it = page_to_ad_bits.find(entry.first);

                if(it == page_to_ad_bits.end()) {
                    page_to_ad_bits[entry.first] = 1;
                } else {
                    page_to_ad_bits[entry.first]++;
                    page_to_ad_bits[entry.first] %= (num_bits +1);
                }
            }
            last_sample_time += sample_time;
            accessed_pages.clear();
        }

        if (pin_ee.eh.time * 1000 - start_time > scale*timing) {  // analysis time,
                                                            // all in ms
            uint64_t least_accesses = -1;
            uint64_t victim_page;
            for (auto& page: current_pages) {
                auto it = page_to_ad_bits.find(page);
                if (it == page_to_ad_bits.end()) {
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
            for (auto &[page, accesses]: page_to_ad_bits) {
                if (accesses > most_accesses) {
                    most_accesses = accesses;
                    promoted_page = page;
                }
            }
            current_pages.insert(promoted_page);
            page_to_ad_bits.clear();
            start_time = pin_ee.eh.time * 1000;
        
        }



        pin_ee = pinTrace.readNextEntry();
    }

    fprintf(outfile, "%ld %d %ld %ld %f\n", total_pages, timing, hits, misses, 
        ((double)misses)/(double)(hits+misses));

} 

int main(int argc, char* argv[]) {

    if (argc < 4) {
        std::cerr << "./dynamic_lfu pin_trace outputfile num_bits sample_time scale numpages..numpages" << std::endl;
        return 1;
    }

    pinTrace.setTraceFile(argv[1]);
//     hotnessTrace.setTraceFile(argv[2]);

//     auto initial_mapping = traceToMap(pinTrace);

    outfile = fopen(argv[2], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[3] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }

    int num_bits = std::stoi(argv[3]);

    int sample_time = std::stoi(argv[4]);   // in seconds
    double scale = std::stoi(argv[5]);   // in seconds

    int arg_num = 6;
//     std::vector<int> page_mvmt_speeds{10, 100, 1000, 5000, 10000, 100000}; // 1 page mvmt per x ms
    std::vector<int> page_mvmt_speeds{1000, 5000, 10000, 50000, 100000}; // 1 page mvmt per x ms
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        std::vector<uint64_t> starting_pages =
        getStartingFIFO(pinTrace, page_limit);
        for (const auto timing : page_mvmt_speeds) {
            pinTrace = TraceFile();
            pinTrace.setTraceFile(argv[1]);

            fprintf(stderr, "calling dynamic lfu!\n");

            dynamicLFU(starting_pages, page_limit, timing, num_bits,
            sample_time, scale);
        }
        arg_num++;
    }
}
