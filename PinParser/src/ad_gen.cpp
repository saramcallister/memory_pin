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

void analyze_trace(int num_bits, int sample_time, int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
    std::unordered_map<uint64_t, uint64_t> page_to_count;
    std::unordered_map<uint64_t, uint64_t> accessed_pages;
    std::unordered_map<uint64_t, uint64_t> page_to_ad_bits;;

    int64_t seen_instr = 0;
    int num_reads = 0;
    int64_t start_instr = 0;
    bool first = true;
    double last_sample_time, last_analyze_time;

    while (instr_limit > seen_instr - start_instr) {
        num_reads ++;
        EntireEntry ee = read_trace.readNextEntry();
        EntireEntry write_ee;
        if (!ee.valid) {
            break;
        }

        write_ee.valid = true;

        if (first) {
            start_instr = ee.eh.instructions;
            first = false;
            last_sample_time = ee.eh.time;
            last_analyze_time = ee.eh.time;
        }

        seen_instr += ee.eh.instructions;
        write_ee.eh.instructions = ee.eh.instructions;
        write_ee.eh.time = ee.eh.time;

        for (auto pe: ee.pe_list) {
//             tot_accesses += pe.accesses;
            auto it = page_to_count.find(pe.page_num);
            if (it == page_to_count.end()) {
                page_to_count[pe.page_num] = pe.accesses;
            } else {
                page_to_count[pe.page_num] += pe.accesses;
            }
            auto it2 = accessed_pages.find(pe.page_num);
            if(it2 == accessed_pages.end()) {
                accessed_pages[pe.page_num] = 1;
            }
        }


        if((ee.eh.time - last_sample_time) >= sample_time) {
            for (auto entry : accessed_pages) {
                auto it = page_to_ad_bits.find(entry.first);

                if(it == page_to_ad_bits.end()) {
                    page_to_ad_bits[entry.first] = 1;
                } else {
                    page_to_ad_bits[entry.first]++;
                    page_to_ad_bits[entry.first] %= (num_bits +1);
                }
            }

            last_sample_time += sample_time;
            accessed_pages.clear(); // clear ad bits every sample cycle
        }

        if((ee.eh.time - last_analyze_time) >= sample_time*num_bits) {
            for(auto it: page_to_ad_bits) {
                PageEntry pe(it.first, it.second);
                write_ee.pe_list.push_back(pe);
            }

            write_ee.eh.num_pages = page_to_ad_bits.size();

            write_trace.writeEntry(write_ee);

            page_to_ad_bits.clear();
            page_to_count.clear();

            last_analyze_time += sample_time*num_bits;

        }

        if (num_reads % 1001 == 1000) {
            std::cerr << '.';
        }
    }

    std::cerr << std::endl;
}

int main(int argc, char* argv[]) {

    if(argc < 4) {
        std::cerr << "./ad_gen read_trace write_trace num_bits scale [sample_freq = 5s]" << std::endl;
        return 1;
    }

//     double sample_freq = 0.01;
    double sample_freq = 5; // 5 seconds

    if(argc == 6) {
        sample_freq = std::stof(argv[5]);
    }

    sample_time = sample_freq*std::stof(argv[4]);

    read_trace.setTraceFile(argv[1]);
    write_trace.setTraceFile(argv[2], false);
    int num_bits = std::stoi(argv[3]);

    analyze_trace(num_bits, sample_time);

}

