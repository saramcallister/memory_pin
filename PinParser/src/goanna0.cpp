#include <algorithm>
#include <assert.h>
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

uint64_t MV_PER_TIME = 200;

static unsigned int gseed=123456789;
inline int fastrand() {
    gseed = (214013 * gseed + 2531011);
    return (gseed >> 16) & 0x7fff;
}

struct SimInfo {
    uint64_t hits;
    uint64_t misses;
    uint64_t num_local_pages;
    std::unordered_set<uint64_t> local_pages;
};

void recordHits(SimInfo& siminfo, std::unordered_map<uint64_t, uint64_t> ptc) {
    for (auto [page, count]: ptc) {
        auto it = siminfo.local_pages.find(page);
        if (it == siminfo.local_pages.end()) {
            siminfo.misses += count;
        } else {
            siminfo.hits += count;
        }
    }
}

void evictRandom(std::unordered_set<uint64_t>& pages, uint64_t num_to_evict) {
    for (uint64_t i = 0; i < num_to_evict; i++) {
        if (!pages.size()) {
            break;
        }
        int evict_candidate = fastrand() % pages.size();
        auto it = pages.begin();
        std::advance(it, evict_candidate);
        pages.erase(it);
    }
}

std::unordered_set<uint64_t> insertHot(
        std::unordered_set<uint64_t>& pages, 
        const std::unordered_map<uint64_t, uint64_t>& ptc,
        uint64_t num_local_pages) {
    std::priority_queue<PageEntry, std::vector<PageEntry>, PageEntryComparator> pq;
    for (const auto & [ page, accesses ] : ptc) {
        pq.push(PageEntry(page, accesses));
    }

    std::unordered_set<uint64_t> to_insert;
    while (to_insert.size() < MV_PER_TIME && pq.size() > 0 && to_insert.size() < num_local_pages) {
        PageEntry pe = pq.top();
        auto it = pages.find(pe.page_num);
        if (it == pages.end()) {
            to_insert.insert(pe.page_num);
        }
        pq.pop();
    }
    return to_insert;
}

void getAccesses(double time_s, std::vector<SimInfo>& sim_tracker) {

    int num_reads = 0;
    double last_time = 0;
    std::unordered_map<uint64_t, uint64_t> pages_to_count;
    std::unordered_set<uint64_t> pages_seen;

    EntireEntry ee = trace.readNextEntry();
    while (ee.valid) {
        num_reads ++;

        if (ee.eh.time - last_time > time_s) {
            for (auto& siminfo: sim_tracker) {
                recordHits(siminfo, pages_to_count);
                auto to_insert = insertHot(siminfo.local_pages, pages_to_count, siminfo.num_local_pages);
                assert(to_insert.size() <= MV_PER_TIME);
                evictRandom(siminfo.local_pages, to_insert.size());
                std::copy(to_insert.begin(), to_insert.end(), inserter(siminfo.local_pages, siminfo.local_pages.begin()));
                assert(siminfo.local_pages.size() <= siminfo.num_local_pages);
            }
            last_time = ee.eh.time;
            pages_to_count.clear();
        }

        for (auto pe: ee.pe_list) {
            auto it = pages_to_count.find(pe.page_num);
            bool not_found = false;
            if (it == pages_to_count.end()) {
                not_found = true;
                pages_to_count[pe.page_num] = std::min<uint64_t>(pe.accesses, 64);
            } else {
                it->second += std::min<uint64_t>(pe.accesses, 64);
            }

            if (not_found) {
                auto set_it = pages_seen.find(pe.page_num);
                if (set_it == pages_seen.end()) {
                    for (auto& siminfo: sim_tracker) {
                        if (siminfo.local_pages.size() < siminfo.num_local_pages) {
                            siminfo.local_pages.insert(pe.page_num);
                        }
                    }
                }
            }
        }
        if (num_reads % 1001 == 1000) {
            fprintf(stderr, ".");
        }
        ee = trace.readNextEntry();
    }
    std::cout << std::endl;
}

    
void outputHitsMisses(std::vector<SimInfo> sim_tracker) {
    for (auto siminfo: sim_tracker) {
        fprintf(outfile, "%ld %ld %ld\n", siminfo.num_local_pages, siminfo.hits, siminfo.misses);
    }
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

    double time_horizon = 1 / 1000; // 1 ms time horizon
    
    int arg_num = 3;
    int total_sims = arg_num;
    std::vector<SimInfo> sim_tracker;
    sim_tracker.resize(argc - arg_num);
    while (total_sims < argc) {
        uint64_t page_limit = atoi(argv[total_sims]);
        sim_tracker[total_sims - arg_num].num_local_pages = page_limit;
        total_sims++;
    }
    getAccesses(time_horizon, sim_tracker);
    outputHitsMisses(sim_tracker);
}
