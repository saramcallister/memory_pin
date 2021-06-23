#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

struct EntryHeader {
    double time;
    uint64_t instructions;
    uint64_t num_pages;
};

struct PageEntry {
    uint64_t page_num;
    uint64_t accesses;

    PageEntry(uint64_t pn, uint64_t a) {
        page_num = pn;
        accesses = a;
    }

    PageEntry() {}
};

class PageEntryComparator {
public:
    int operator() (const PageEntry& pe1, const PageEntry& pe2) {
        return pe1.accesses < pe2.accesses;
    }
};

FILE* trace;

struct EntireEntry {
    EntryHeader eh;
    std::vector<PageEntry> pe_list;
    bool valid;
};

EntireEntry readFormatted() {
    EntireEntry ee;
    ee.valid = true;
    size_t ret = fread(&ee.eh, sizeof(ee.eh), 1, trace);
    if (ret != 1) {
        ee.valid = false;
        return ee;
    }

    if (ee.eh.num_pages == 0) {
        return ee;
    }

    PageEntry pe;
    std::vector<PageEntry> pe_list;
    ee.pe_list.reserve(ee.eh.num_pages);
    for (uint64_t i = 0; i < ee.eh.num_pages; i++) {
        ret = fread(&pe, sizeof(pe), 1, trace);
        if (ret != 1) {
            ee.valid = false;
            return ee;
        }
        ee.pe_list.push_back(pe);
    }
    return ee;
}

std::unordered_map<uint64_t, uint64_t> aggregatePages(int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
    std::unordered_map<uint64_t, uint64_t> page_to_count;
    int64_t seen_instr = 0;
    int num_reads = 0;
    int64_t start_instr = 0;
    bool first = true;
    while (instr_limit > seen_instr - start_instr) {
        num_reads ++;
        EntireEntry ee = readFormatted();
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
            std::cout << '.';
        }
    }
    std::cout << std::endl;
    return page_to_count;
}

void getPopularPages(std::unordered_map<uint64_t, uint64_t> aggregated_pages, uint64_t total_pages) {
    std::priority_queue <PageEntry, std::vector<PageEntry>, PageEntryComparator> pq;
    for (auto & [ page_num, accesses ] : aggregated_pages) {
        pq.push(PageEntry(page_num, accesses));
    }

    uint64_t misses = 0;
    uint64_t hits = 0;
    uint64_t pages_taken = 0;
    while (pq.empty() == false) {
        PageEntry pe = pq.top();
        if (pages_taken < total_pages) {
            hits += pe.accesses;
        } else {
            misses += pe.accesses;
        }
        pages_taken++;
        pq.pop();
    }

    fprintf(stdout, "Hits: %ld Misses: %ld Miss Ratio: %f\n", hits, misses, misses / (double) (hits + misses));
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "./lfu tracefile numpages..numpages" << std::endl;
        return 1;
    }

    trace = fopen(argv[1], "r");

    auto mapping = aggregatePages();

    int arg_num = 2;
    while (arg_num < argc) {
        uint64_t page_limit = atoi(argv[arg_num]);
        fprintf(stdout, "Page Limit: %ld ---- ", page_limit);
        getPopularPages(mapping, page_limit);
        arg_num++;
    }
}

