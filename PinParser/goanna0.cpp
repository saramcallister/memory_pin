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

uint64_t MV_PER_TIME = 200;

struct PinInfo {
    std::vector<std::unordered_map<uint64_t, uint64_t>> ptc;
    std::vector<uint64_t> page_order;
};

PinInfo getAccesses(double time_s) {
    PinInfo info;

    int num_reads = 0;
    uint64_t index = 0;
    double last_time = 0;
    std::unordered_set<uint64_t> pages_seen;

    info.ptc.push_back(std::unordered_map<uint64_t, uint64_t>());

    EntireEntry ee = trace.readNextEntry();
    while (ee.valid) {
        num_reads ++;

        if (ee.eh.time - last_time > time_s) {
            info.ptc.push_back(std::unordered_map<uint64_t, uint64_t>());
            index++;
            last_time = ee.eh.time;
        }

        for (auto pe: ee.pe_list) {
            auto it = info.ptc[index].find(pe.page_num);
            if (it == info.ptc[index].end()) {
                info.ptc[index][pe.page_num] = std::min<uint64_t>(pe.accesses, 64);
            } else {
                it->second += std::min<uint64_t>(pe.accesses, 64);
            }
            
            auto set_it = pages_seen.find(pe.page_num);
            if (set_it == pages_seen.end()) {
                info.page_order.push_back(pe.page_num);
                pages_seen.insert(pe.page_num);
            }
        }
        if (num_reads % 10001 == 10000) {
            fprintf(stderr, ".");
        }
        ee = trace.readNextEntry();
    }
    std::cout << std::endl;

    return info;
}

static unsigned int gseed=123456789;
inline int fastrand() {
    gseed = (214013 * gseed + 2531011);
    return (gseed >> 16) & 0x7fff;
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

std::unordered_set<uint64_t> insertHot(std::unordered_set<uint64_t>& pages, const std::unordered_map<uint64_t, uint64_t>& ptc) {
    std::priority_queue<PageEntry, std::vector<PageEntry>, PageEntryComparator> pq;
    for (const auto & [ page, accesses ] : ptc) {
        pq.push(PageEntry(page, accesses));
    }

    std::unordered_set<uint64_t> to_insert;
    while (to_insert.size() < MV_PER_TIME && pq.size() > 0 && to_insert.size() < pages.size()) {
        PageEntry pe = pq.top();
        auto it = pages.find(pe.page_num);
        if (it == pages.end()) {
            to_insert.insert(pe.page_num);
        }
        pq.pop();
    }
    return to_insert;
}
    
void outputHitsMisses(uint64_t num_local_pages, const PinInfo& info) {
    std::unordered_set<uint64_t> current_pages;
    std::copy(info.page_order.begin(), info.page_order.begin() + num_local_pages, inserter(current_pages, current_pages.begin()));

    uint64_t misses = 0;
    uint64_t hits = 0;
    for (const auto& pages_to_count: info.ptc) {
        for (const auto& [page, count]: pages_to_count) {
            auto it = current_pages.find(page);
            if (it == current_pages.end()) {
                misses += count;
            } else {
                hits += count;
            }
        }

        auto to_insert = insertHot(current_pages, pages_to_count);
        evictRandom(current_pages, to_insert.size());
        std::copy(to_insert.begin(), to_insert.end(), inserter(current_pages, current_pages.begin()));
    }
    fprintf(outfile, "%ld %ld %ld\n", num_local_pages, hits, misses);
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
    
    auto info = getAccesses(time_horizon);

    int arg_num = 3;
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        outputHitsMisses(page_limit, info);
        arg_num++;
    }
}
