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
#define PEBS_PERIOD 0.01
#define HOT_PAGE_LEN 1000

uint64_t total_pebs;
uint64_t total_truth;
std::fstream fptr;
float percent_hot_pages;

bool cmp ( std::pair <uint64_t, uint64_t> a, std::pair <uint64_t, uint64_t> b) {
    return a.second > b.second;
}
std::vector<std::pair<uint64_t, uint64_t>> sort ( 
                            std::unordered_map<uint64_t, uint64_t> map) {
    std::vector<std::pair<uint64_t, uint64_t>> v;

    for(auto &it:map) {
        v.push_back(it);
    }

    sort(v.begin(), v.end(), cmp);

    return v;
}

void checkSimilarity(std::unordered_map<uint64_t, uint64_t> truth,
std::unordered_map<uint64_t, HotnessEntry> hot_pages) {
    std::unordered_set<uint64_t> page_set;
    uint64_t pg_count = 0;
    uint64_t count = 0;

    uint64_t limit = truth.size()*percent_hot_pages;
    auto sorted_truth = sort(truth);

    for( auto i : sorted_truth) {

       if(count < limit) {
           page_set.insert(i.first);
           count++;
       }
       else {
           break;
       }

    }

    for(auto i: page_set) {
        if(hot_pages.find(i) != hot_pages.end()) {
            pg_count++;
        }
    }

    fprintf(stdout, "hotness mechanism could identify %f of the top %f%% pages\n",
    ((float) pg_count/(sorted_truth.size()*percent_hot_pages)), percent_hot_pages*100);
    fprintf(stdout, 
        "pg_count: %ld, sorted_truth.size(): %ld, hot_pages.size(): %ld\n", 
            pg_count, sorted_truth.size(), hot_pages.size());

}


TraceFile trace;

void analyze_trace(bool gen_file_data, float time_scale,
                    int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
    std::unordered_map<uint64_t, uint64_t> page_to_count;
    std::unordered_map<uint64_t, HotnessEntry> hot_pages;   //page num to time
                                                            // and accesses

    int64_t seen_instr = 0;
    int num_reads = 0;
    int64_t start_instr = 0;
    bool first = true;
//     uint64_t access_counter=0;
    double time_stamp;

    while (instr_limit > seen_instr - start_instr) {
        num_reads ++;
        EntireEntry ee = trace.readNextEntry();
        if (!ee.valid) {
            break;
        }

        uint64_t tot_accesses = 0;

        if (first) {
            start_instr = ee.eh.instructions;
            first = false;
            time_stamp = ee.eh.time;
        }

        seen_instr += ee.eh.instructions;


        for (auto pe: ee.pe_list) {
            tot_accesses += pe.accesses;
            auto it = page_to_count.find(pe.page_num);
            if (it == page_to_count.end()) {
                page_to_count[pe.page_num] = pe.accesses;
            } else {
                page_to_count[pe.page_num] += pe.accesses;
            }
            total_truth += pe.accesses;

            auto entry = hot_pages.find(pe.page_num);

            if(entry == hot_pages.end()) {
                if(hot_pages.size() >= HOT_PAGE_LEN) {
                    double oldest_time = ee.eh.time;    // curr time
                    uint64_t page_num = -1;
                    for(auto it: hot_pages) {
                       if(it.second.time <= oldest_time) { // last entry of current
                                                            // time gets counter, 
                                                            //should make this random
                           page_num = it.first;
                           oldest_time = it.second.time;
                       }
                    }

                    hot_pages.erase(page_num);

                }

                HotnessEntry he(pe.accesses, ee.eh.time);
                hot_pages[pe.page_num] = he;


            } else {
                hot_pages[pe.page_num].accesses += 1;
                hot_pages[pe.page_num].time = ee.eh.time;   // upadted time
            }
            
        }

        if(ee.eh.time - time_stamp >= time_scale) {
            checkSimilarity(page_to_count, hot_pages);
//             hot_pages.clear();
            page_to_count.clear();
            time_stamp = ee.eh.time;
        }

        if (num_reads % 1001 == 1000) {
            std::cerr << '.';
        }
    }

    std::cerr << std::endl;
}

int main(int argc, char* argv[]) {

    if(argc < 5) {
        std::cerr << "./pebs tracefile percent_hot_pages time_scale sample_time" << std::endl;
        return 1;
    }

    trace.setTraceFile(argv[1]);

    float percent_input = std::stoi(argv[2]);

    float time_scale = std::stof(argv[3]);

    float sample_time = std::stof(argv[4]);

    percent_hot_pages = percent_input/100;
    fprintf(stdout, "percent hot pages: %f\n", percent_hot_pages);
    fprintf(stdout, "time_scale: %f\n", time_scale);
    fprintf(stdout, "sample_time: %f\n", sample_time);

    analyze_trace(false,  time_scale);

}

