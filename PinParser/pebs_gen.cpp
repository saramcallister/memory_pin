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
TraceFile write_trace;
float sample_time;

void analyze_trace(int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
    std::unordered_map<uint64_t, uint64_t> page_to_count;
    std::unordered_map<uint64_t, uint64_t> pebs_count;

    int64_t seen_instr = 0;
    int num_reads = 0;
    int64_t start_instr = 0;
    bool first = true;
    uint64_t access_counter=0;

    while (instr_limit > seen_instr - start_instr) {
        num_reads ++;
        EntireEntry ee = read_trace.readNextEntry();
        EntireEntry write_ee;
        if (!ee.valid) {
            break;
        }

        write_ee.valid = true;

        uint64_t tot_accesses = 0;

        if (first) {
            start_instr = ee.eh.instructions;
            first = false;
        }

        seen_instr += ee.eh.instructions;
        write_ee.eh.instructions = ee.eh.instructions;
        write_ee.eh.time = ee.eh.time;

        for (auto pe: ee.pe_list) {
            tot_accesses += pe.accesses;
            auto it = page_to_count.find(pe.page_num);
            if (it == page_to_count.end()) {
                page_to_count[pe.page_num] = pe.accesses;
            } else {
                page_to_count[pe.page_num] += pe.accesses;
            }
        }

        int num_samples = (access_counter + tot_accesses) / PEBS_FREQ;

        std::random_device r;
        std::default_random_engine e1(r());
        std::uniform_int_distribution<uint64_t> uniform_dist(1, tot_accesses);

        for(int i = 0; i < num_samples; i++) {
            uint64_t aggregate = 0;
               
            uint64_t mean = uniform_dist(e1);
            for(auto pe: ee.pe_list) {
                aggregate += pe.accesses;

                if(aggregate >= mean) {
                    auto it = pebs_count.find(pe.page_num);

                    if( it == pebs_count.end()) {
                        pebs_count[pe.page_num] = 1;
                    } else {
                        it->second = std::min<uint64_t>(it->second+1, 64);
                    }
                    break;
                }
                
            }

        }

        for( auto it: pebs_count) {
            PageEntry pe(it.first, it.second);
            write_ee.pe_list.push_back(pe);
        }

        write_ee.eh.num_pages = pebs_count.size();

        write_trace.writeEntry(write_ee);

        pebs_count.clear();
        page_to_count.clear();

        access_counter += tot_accesses;
        access_counter = access_counter % PEBS_FREQ;

        if (num_reads % 1001 == 1000) {
            std::cerr << '.';
        }
    }

    std::cerr << std::endl;
}

int main(int argc, char* argv[]) {

    if(argc < 4) {
        std::cerr << "./pebs_gen read_trace write_trace scale [sample_freq = 10ms]" << std::endl;
        return 1;
    }

    double sample_freq = 0.01;

    if(argc == 5) {
        sample_freq = std::stof(argv[4]);
    }

    sample_time = sample_freq*std::stof(argv[3]);

    read_trace.setTraceFile(argv[1]);
    write_trace.setTraceFile(argv[2], false);

    analyze_trace();

}

