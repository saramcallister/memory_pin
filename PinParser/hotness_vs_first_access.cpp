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
#include <utility>
#include <vector>

#include "parser.hpp"

TraceFile trace;

std::unordered_map<uint64_t, std::pair<uint64_t, double>> aggregatePages(int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
    std::unordered_map<uint64_t, std::pair<uint64_t, double>> page_to_count_time;
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
            auto it = page_to_count_time.find(pe.page_num);
            if (it == page_to_count_time.end()) {
                page_to_count_time[pe.page_num] = std::make_pair(std::min<uint64_t>(pe.accesses, 64), ee.eh.time);
            } else {
                it->second.first += std::min<uint64_t>(pe.accesses, 64);
            }
        }
        if (num_reads % 1001 == 1000) {
            //fprintf(stdout, "Read through %d entries, %ld instructions\n", num_reads, seen_instr - start_instr);
            std::cerr << '.';
        }
    }
    std::cerr << std::endl;
    return page_to_count_time;
}


int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "./hfa tracefile outfile" << std::endl;
        return 1;
    }

    trace.setTraceFile(argv[1]);
    
    FILE* outfile = fopen(argv[2], "w");
    if (!outfile) {
        std::cerr << "Failed to open output file (" << argv[2] << ") for writing"
            << " with error: " << strerror(errno) << std::endl;
        exit(1);
    }

    auto mapping = aggregatePages();
    for (auto [page, p] : mapping) {
        fprintf(outfile, "%ld %f\n", p.first, p.second);
    }

}

