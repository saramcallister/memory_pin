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

#include "parser.hpp"
// #define PEBS_FREQ 5000
// #define SAMPLE_TIME_IN_S 10 // TODO: made tunable
// #define SAMPLE_TIME_IN_S 5 // TODO: made tunable
// #define PIN_TIME_SCALE 30 // TODO: make this tunable - per workload? currently
                          // only for 505
// #define PIN_TIME_SCALE 63.5
#define MAX_BITS 1 // TODO: made tunable 
#define MAX_SAMPLE_TIME (SAMPLE_TIME_IN_S * PIN_TIME_SCALE * MAX_BITS)

std::vector<std::pair<uint64_t, uint64_t>> sorted_ad;
std::vector<std::pair<uint64_t, uint64_t>> sorted_truth;

std::vector<std::pair<uint64_t, uint64_t>> pebs_read;
std::vector<std::pair<uint64_t, uint64_t>> truth_read;
uint64_t total_pebs;
uint64_t total_truth;
std::fstream fptr;

float percent_hot_pages;

bool cmp ( std::pair <uint64_t, uint64_t> a, std::pair <uint64_t, uint64_t> b) {
    return a.second > b.second;
}

void checkSimilarity(int num_bits) {

    std::unordered_set<uint64_t> page_set;
    std::unordered_set<uint64_t> top_ad_pages;
    uint64_t pg_count = 0;
    uint64_t count = 0;

//     uint64_t limit = sorted_truth.size()/10;
    uint64_t limit = sorted_truth.size()*percent_hot_pages;

    for( auto i : sorted_truth) {

       if(count < limit) {
           page_set.insert(i.first);
           count++;
       }
       else {
           break;
       }

    }

    count = 0;
//     limit = sorted_ad.size()/10;
    limit = sorted_ad.size()*percent_hot_pages;

    uint64_t curr_count = num_bits;
    for( auto i : sorted_ad) {
        if(i.second < curr_count) {
            if(count >= limit) {
                break;      // we have already added enough pages
            } else {
                curr_count = i.second;
            }
        }

        if(i.second > curr_count) {
            fprintf(stderr, "THIS SHOULDN'T BE REACHED\n");
            exit(-1);
        }
        count ++;

        top_ad_pages.insert(i.first);

    }

    for(auto i: top_ad_pages) {
        if(page_set.find(i) != page_set.end()) {
            pg_count++;
        }
    }

    fprintf(stdout, "ad could identify %f of the top %f%% pages\n",
    ((float) pg_count/(sorted_truth.size() * percent_hot_pages)), percent_hot_pages*100);
    fprintf(stdout, 
        "pg_count: %ld, sorted_truth.size(): %ld, sorted_ad.size(): %ld\n", 
            pg_count, sorted_truth.size(), sorted_ad.size());

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

TraceFile trace;

void analyze_trace(bool gen_file_data, int num_bits, float sample_time, 
                    int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
    std::unordered_map<uint64_t, uint64_t> page_to_count;
    std::unordered_map<uint64_t, uint64_t> page_to_ad_bits;

    std::unordered_set<uint64_t> current_pages; 
//     std::unordered_set<uint64_t> accessed_pages;
    std::unordered_map<uint64_t, uint64_t> accessed_pages;

    int64_t seen_instr = 0;
    int num_reads = 0;
    int64_t start_instr = 0;
    bool first = true;
    double last_sample_time, last_analyze_time; 
//     int sample_time = SAMPLE_TIME_IN_S * PIN_TIME_SCALE * num_bits;

    while (instr_limit > seen_instr - start_instr) {
        num_reads ++;
        EntireEntry ee = trace.readNextEntry();
        if (!ee.valid) {
            break;
        }

        if (first) {
            start_instr = ee.eh.instructions;
            first = false;
            last_sample_time = ee.eh.time;
            last_analyze_time = ee.eh.time;
        }

        seen_instr += ee.eh.instructions;

        for (auto pe: ee.pe_list) {

            auto it = page_to_count.find(pe.page_num);
            if (it == page_to_count.end()) {
                page_to_count[pe.page_num] = pe.accesses;
            } else {
                page_to_count[pe.page_num] += pe.accesses;
            }
            total_truth += pe.accesses;

            auto it2 = accessed_pages.find(pe.page_num);
            if (it2 == accessed_pages.end()) {   // when using a page, mark it as
                                            //accessed
//                 accessed_pages.insert(pe.page_num);
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
            sorted_ad = sort(page_to_ad_bits);
            sorted_truth = sort(page_to_count);

            checkSimilarity(num_bits);

            last_analyze_time += sample_time*num_bits;

            page_to_count.clear();
            page_to_ad_bits.clear();

            sorted_ad.clear();
            sorted_truth.clear();

        }
        
        if (num_reads % 1001 == 1000) {
            std::cerr << '.';
        }
    }

    std::cerr << std::endl;
}

int main(int argc, char* argv[]) {

    if(argc < 6) {
        std::cerr << "./ad tracefile percent_hot_pages num_bits time_scale sample_time" << std::endl;
        return 1;
    }

    trace.setTraceFile(argv[1]);

    float percent_input = std::stoi(argv[2]);

    int num_bits = std::stoi(argv[3]);
    
    float time_scale = std::stof(argv[4]);

    int sample_time = std::stoi(argv[5]);

    percent_hot_pages = percent_input/100;
    fprintf(stdout, "percent hot pages: %f\n", percent_hot_pages);
    fprintf(stdout, "num_bits: %d\n", num_bits);
    fprintf(stdout, "time_scale: %f\n", time_scale);
    fprintf(stdout, "sample_time: %d\n", sample_time);

    analyze_trace(false, num_bits, time_scale*sample_time);

}

