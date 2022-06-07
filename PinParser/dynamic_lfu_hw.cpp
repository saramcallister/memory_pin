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

#define HOT_PAGE_LEN 1000

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
            trace_map.push_back(pe);
        }
        if (num_reads % 10001 == 10000) {
            fprintf(stderr, ".");
        }
    }
    std::cout << std::endl;
    return trace_map;
}


std::vector<uint64_t> getStartingLFU(std::vector<PageEntry> map, uint64_t total_pages) {
    std::unordered_map<uint64_t, uint64_t> initial;

    for(auto pe: map) {
        auto it = initial.find(pe.page_num);
        if (it == initial.end()) {
            initial[pe.page_num] = std::min<uint64_t>(pe.accesses, 64);
        } else {
            it->second += std::min<uint64_t>(pe.accesses, 64);
        }
    }

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

void dynamicLFU(std::vector<uint64_t> starting_pages, uint64_t total_pages, int
timing, double scale) {
    EntireEntry pin_ee = pinTrace.readNextEntry();

    std::unordered_set<uint64_t> current_pages;
    std::unordered_map<uint64_t, uint64_t> accessed_pages;
//     std::unordered_map<uint64_t, uint64_t> page_to_ad_bits;
    std::unordered_map<uint64_t, HotnessEntry> hot_pages;

//     uint64_t last_sample_time = pin_ee.eh.time;

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

            auto entry = hot_pages.find(pe.page_num);

            if(entry == hot_pages.end()) {
                if(hot_pages.size() >= HOT_PAGE_LEN) {
                    double oldest_time = pin_ee.eh.time;
                    uint64_t page_num = -1;
                    for(auto it:hot_pages) {
                        if(it.second.time <= oldest_time) {
                            page_num = it.first; 
                            oldest_time = it.second.time;
                        }
                    }

                    hot_pages.erase(page_num);
                }

                HotnessEntry he(pe.accesses, pin_ee.eh.time);
                hot_pages[pe.page_num] = he;
            } else {
                hot_pages[pe.page_num].accesses += 1;
                hot_pages[pe.page_num].time = pin_ee.eh.time * 1000 * 1000;
            }
        }

        if (pin_ee.eh.time * 1000 * 1000 - start_time > scale*timing) {  // analysis time,
                                                            // all in us
            uint64_t least_accesses = -1;
            uint64_t victim_page;
            for (auto& page: current_pages) {
                auto it = hot_pages.find(page);
                if (it == hot_pages.end()) {
                    victim_page = page; 
                    break;
                } else if (it->second.accesses < least_accesses) {
                    victim_page = page;
                    least_accesses = it->second.accesses;
                }
            }
            current_pages.erase(victim_page);

            uint64_t most_accesses = 0;
            uint64_t promoted_page;
            for (auto &[page, he]: hot_pages) {
                if (he.accesses > most_accesses) {
                    most_accesses = he.accesses;
                    promoted_page = page;
                }
            }
            current_pages.insert(promoted_page);
//             hot_pages.clear();
            start_time = pin_ee.eh.time * 1000 * 1000;
        
        }

        pin_ee = pinTrace.readNextEntry();
    }

    fprintf(outfile, "%ld %d %ld %ld %f\n", total_pages, timing, hits, misses, 
        ((double)misses)/(double)(hits+misses));

} 

int main(int argc, char* argv[]) {

    if (argc < 4) {
        std::cerr << "./dynamic_lfu pin_trace outputfile scale numpages..numpages" << std::endl;
        return 1;
    }

    pinTrace.setTraceFile(argv[1]);

    auto initial_mapping = traceToMap(pinTrace);

    outfile = fopen(argv[2], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[3] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }

//     int num_bits = std::stoi(argv[3]);

//     int sample_time = std::stoi(argv[4]);   // in seconds
    double scale = std::stof(argv[3]);   // in seconds


    int arg_num = 4;
    std::vector<int> page_mvmt_speeds{10, 50, 100}; // 1 page mvmt per x us
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        std::vector<uint64_t> starting_pages =
        getStartingLFU(initial_mapping, page_limit);
        for (const auto timing : page_mvmt_speeds) {
            pinTrace = TraceFile();
            pinTrace.setTraceFile(argv[1]);

            dynamicLFU(starting_pages, page_limit, timing,scale);
        }
        arg_num++;
    }
}
