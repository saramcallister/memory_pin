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
#define HOT_PAGE_LEN 1000

TraceFile pin_trace;
TraceFile hotness_trace;
FILE* outfile;

std::unordered_map<uint64_t, uint64_t> aggregatePages(TraceFile &trace) {
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
    EntireEntry pin_ee = pin_trace.readNextEntry();

    std::unordered_set<uint64_t> current_pages;
    std::copy(starting_pages.begin(), starting_pages.end(), inserter(current_pages, current_pages.begin()));

    uint64_t misses = 0;
    uint64_t hits = 0;

    uint64_t pages_moved = 0;

    double start_time = pin_ee.eh.time * 1000; // seconds to ms
//     std::unordered_map<uint64_t, double> hits_to_time;
    std::unordered_map<uint64_t, HotnessEntry> hot_pages;

    while (pin_ee.valid) {

        if (pin_ee.eh.time * 1000 - start_time > scale*timing) {
            double min_time = pin_ee.eh.time;
            uint64_t victim_page;
            for (auto& page: current_pages) {
                auto it = hot_pages.find(page);
                if (it == hot_pages.end()) {
                    victim_page = page; 
                    break;
                } else if (it->second.time < min_time) {
                    victim_page = page;
                    min_time = it->second.time;
                }
            }
            current_pages.erase(victim_page);

            double max_time = -1;
            uint64_t promoted_page;
            for (auto &[page, entry]: hot_pages) {
                if ( entry.time > max_time) {
                    max_time = entry.time;
                    promoted_page = page;
                }
            }
            current_pages.insert(promoted_page);
            pages_moved++;
//             hits_to_time.clear();
            start_time = pin_ee.eh.time * 1000;
        
        }

        for (auto pe: pin_ee.pe_list) {
            if (current_pages.find(pe.page_num) == current_pages.end()) {
                misses += pe.accesses;
            }  else {
                hits += pe.accesses;
            }

            auto it = hot_pages.find(pe.page_num);
            if(it == hot_pages.end()) {
                if(hot_pages.size() > HOT_PAGE_LEN) {
                    double oldest_time = pin_ee.eh.time;
                    uint64_t page_num = -1;

                    for(auto it: hot_pages) {
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
                hot_pages[pe.page_num].time = pin_ee.eh.time;

            }
        }

//         for(auto pe: hotness_ee.pe_list) {
//             auto hit_tracking = hits_to_time.find(pe.page_num);
//             if (hit_tracking == hits_to_time.end()) {
//                 hits_to_time[pe.page_num] = hotness_ee.eh.time;
//             } else {
//                 hit_tracking->second = hotness_ee.eh.time;
//             }
// 
//         }

        pin_ee = pin_trace.readNextEntry();
    }

    fprintf(outfile, "%ld %d %lu %ld %ld %f\n", total_pages, timing,
    pages_moved, hits, misses,
    ((double)misses/(double(hits+misses))));

} 

int main(int argc, char* argv[]) {

    if (argc < 5) {
        std::cerr << "./lru pin_trace outputfile time_scale numpages..numpages" << std::endl;
        return 1;
    }

    pin_trace.setTraceFile(argv[1]);
//     hotness_trace.setTraceFile(argv[2]);

    auto initial_mapping = aggregatePages(pin_trace);

    outfile = fopen(argv[2], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[2] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }

    double scale = std::stof(argv[3]);

    int arg_num = 4;
//     std::vector<int> page_mvmt_speeds{10, 100, 1000, 5000, 10000, 100000}; // 1 page mvmt per x ms
    std::vector<int> page_mvmt_speeds{100, 1000, 10000}; // 1 page mvmt per x us
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        std::vector<uint64_t> starting_pages = getStartingPages(initial_mapping, page_limit);
        for (const auto timing : page_mvmt_speeds) {
            pin_trace = TraceFile();
            pin_trace.setTraceFile(argv[1]); // reset to run through again

            lru(starting_pages, page_limit, timing, scale);
        }
        arg_num++;
    }
}
