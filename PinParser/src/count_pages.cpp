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
#include <unordered_set>
#include <errno.h>
#include <random>

#include "parser.hpp"
#define PEBS_FREQ 5000

TraceFile read_trace;

void analyze_trace(int64_t instr_limit = std::numeric_limits<int64_t>::max()) {

    int64_t seen_instr = 0;
    int num_reads = 0;
    int64_t start_instr = 0;
    bool first = true;
    std::unordered_set<uint64_t> page_list;

    while (instr_limit > seen_instr - start_instr) {
        num_reads ++;
        EntireEntry ee = read_trace.readNextEntry();
        if (!ee.valid) {
            break;
        }

        if (first) {
            start_instr = ee.eh.instructions;
            first = false;
        }

        seen_instr += ee.eh.instructions;

        for (auto pe: ee.pe_list) {
            page_list.insert(pe.page_num);
        }

        if (num_reads % 1001 == 1000) {
            std::cerr << '.';
        }
    }

    printf("total pages: %lu\n", page_list.size());

    std::cerr << std::endl;
}

int main(int argc, char* argv[]) {

    if(argc < 2) {
        std::cerr << "./count_pages tracefile" << std::endl;
        return 1;
    }

    read_trace.setTraceFile(argv[1]);

    analyze_trace();

}

