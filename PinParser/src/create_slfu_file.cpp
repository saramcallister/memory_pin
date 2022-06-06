#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
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

TraceFile trace;
FILE* outfile;

std::unordered_map<uint64_t, uint64_t> aggregatePages(int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
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

void writePopularPages(std::unordered_map<uint64_t, uint64_t> aggregated_pages) {
    std::priority_queue <PageEntry, std::vector<PageEntry>, PageEntryComparator> pq;
    for (auto & [ page_num, accesses ] : aggregated_pages) {
        pq.push(PageEntry(page_num, accesses));
    }

    unsigned long total_pages = aggregated_pages.size();
    std::cout << total_pages << std::endl;
    fwrite(&total_pages, sizeof(unsigned long), 1, outfile);
    while (pq.empty() == false) {
        PageEntry pe = pq.top();
        unsigned long page_num = pe.page_num;
        fwrite(&page_num, sizeof(unsigned long), 1, outfile);
        pq.pop();
    }
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "./create_slfu_file tracefile outfile" << std::endl;
        return 1;
    }

    trace.setTraceFile(argv[1]);
    outfile = fopen(argv[2], "w");
    if (outfile == NULL) {
        std::cerr << "Failed to open (" << argv[2] << ") for writing"
            << " with error: " << std::strerror(errno) << std::endl;
        exit(1);
    }

    auto mapping = aggregatePages();
    writePopularPages(mapping);
}

