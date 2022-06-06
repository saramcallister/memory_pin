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
TraceFile pin_trace;
TraceFile hotness_trace;

std::unordered_map<uint64_t, uint64_t>  sorted_pin;
std::unordered_map<uint64_t, uint64_t> sorted_hotness;

bool cmp ( std::pair <uint64_t, uint64_t> a, std::pair <uint64_t, uint64_t> b) {
    return a.second < b.second;
}

std::vector<uint64_t> get_top_pages (std::priority_queue <PageEntry,
std::vector<PageEntry>, PageEntryComparator> pq, uint64_t num_pages) {

    uint64_t count = 0;
    uint64_t curr_count = std::numeric_limits<uint64_t>::max();

    std::vector<uint64_t> top_pages;

    while(pq.empty() == false) {
        PageEntry pe = pq.top();
        if(pe.accesses < curr_count) {
            if(count >= num_pages) {
                break; 
            } else {
                curr_count = pe.accesses;
            }
        }

        if( pe.accesses > curr_count) {
            fprintf(stderr, "THIS SHOULD NEVER BE REACHED sorted %lu, %lu\n",
            pe.accesses, curr_count);
            exit(-1);
        }

        count++;
        top_pages.push_back(pe.page_num);
        pq.pop();
    }

    fprintf(stdout, "added %lu pages, limit %lu\n", count, num_pages);

    return top_pages;
}


std::unordered_map<uint64_t, uint64_t> aggregatePages(TraceFile &trace, int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
    std::unordered_map<uint64_t, uint64_t> page_to_count;
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
            auto it = page_to_count.find(pe.page_num);
            if (it == page_to_count.end()) {
                page_to_count[pe.page_num] = std::min<uint64_t>(pe.accesses, 64);
            } else {
                it->second += std::min<uint64_t>(pe.accesses, 64);
            }
        }
        if (num_reads % 1001 == 1000) {
            //fprintf(stdout, "Read through %d entries, %ld instructions\n", num_reads, seen_instr - start_instr);
            std::cerr << '.';
        }
    }
    std::cerr << std::endl;
    return page_to_count;
}

void getPopularPages(std::unordered_map<uint64_t, uint64_t> pin_pages,
 std::unordered_map<uint64_t, uint64_t> hotness_pages, uint64_t total_pages) {

    std::priority_queue <PageEntry, std::vector<PageEntry>, PageEntryComparator>
    pq_pin;
    std::priority_queue <PageEntry, std::vector<PageEntry>, PageEntryComparator>
    pq_hotness;

    for (auto & [ page_num, accesses ] : pin_pages) {
        pq_pin.push(PageEntry(page_num, accesses));
    }
    for (auto & [ page_num, accesses ] : hotness_pages) {
        pq_hotness.push(PageEntry(page_num, accesses));
    }

    auto top_pages = get_top_pages(pq_hotness, total_pages);

    uint64_t misses = 0;
    uint64_t hits = 0;
    uint64_t pages_taken = 0;
    while (pq_pin.empty() == false) {
        PageEntry pe = pq_pin.top();
        if (pages_taken < total_pages && (std::find(top_pages.begin(),
            top_pages.end(),pe.page_num) !=
            top_pages.end()) ) {    // hit if we have enough pages and the
                                    //page is hot according to hotness mechanism
            hits += pe.accesses;
        } else {
            misses += pe.accesses;
        }
        pages_taken++;
        pq_pin.pop();
    }

    fprintf(stdout, "Hits + misses: %ld, pages_taken: %ld\n", hits+misses,
    pages_taken);

    fprintf(stdout, "Hits: %ld Misses: %ld Miss Ratio: %f\n", hits, misses, misses / (double) (hits + misses));
}

int main(int argc, char* argv[]) {


    if (argc < 4) {
        std::cerr << "./lfu ground_truth_trace input_trace numpages..numpages" << std::endl;
        return 1;
    }

    pin_trace.setTraceFile(argv[1], true);
    hotness_trace.setTraceFile(argv[2], true); // trace from hotness tracking mechanism

    auto pin_mapping = aggregatePages(pin_trace);
    auto hotness_mapping = aggregatePages(hotness_trace);

    int arg_num = 3;
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        fprintf(stdout, "Page Limit: %ld ---- \n", page_limit);
        getPopularPages(pin_mapping, hotness_mapping, page_limit);
        arg_num++;
    }

}

