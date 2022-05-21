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
#define SAMPLE_TIME_IN_S 5 // TODO: made tunable
#define PIN_TIME_SCALE 30 // TODO: make this tunable - per workload?
#define MAX_ITERATIONS 10 // TODO: made tunable 

std::vector<std::pair<uint64_t, uint64_t>> sorted_ad;
std::vector<std::pair<uint64_t, uint64_t>> sorted_truth;

std::vector<std::pair<uint64_t, uint64_t>> pebs_read;
std::vector<std::pair<uint64_t, uint64_t>> truth_read;
uint64_t total_pebs;
uint64_t total_truth;
std::fstream fptr;

bool cmp ( std::pair <uint64_t, uint64_t> a, std::pair <uint64_t, uint64_t> b) {
    return a.second > b.second;
}

// void read_from_sorted() {
//     std::pair<uint64_t, uint64_t> read;
//     bool read_pebs = true;
// 
//     fptr.seekg(0, fptr.beg);
// 
//     while(fptr) {
//         fptr.read((char *)&read, sizeof(read));
//         if(fptr) {
//             if(read.second == 0) {
//                 read_pebs = false;
//                 continue;
//             }
//             if(read_pebs) {
//                 sorted_pebs.push_back(read);
//             } else {
//                 sorted_truth.push_back(read);
//             }
//         }
//     }
// 
//     for( auto it : sorted_pebs) {
//         total_pebs += it.second;
//     }
//     for( auto it : sorted_truth) {
//         total_truth += it.second;
//     }
// 
// }

// void compare() {
// 
//     for (auto it: truth_read) {
//         auto found = std::find(sorted_truth.begin(), sorted_truth.end(),
//         it);
// 
//         if( found == sorted_truth.end() || it.second != found->second) {
//             fprintf(stderr, "read and sorted is not the same\n");
//             exit(1);
//         }
//     }
// 
//     for (auto it: pebs_read) {
//         auto found = std::find(sorted_pebs.begin(), sorted_pebs.end(),
//         it);
// 
//         if( found == sorted_pebs.end() || it.second != found->second) {
//             fprintf(stderr, "read and sorted is not the same\n");
//             exit(1);
//         }
//     }
// 
// }

void checkSimilarity() {

    std::unordered_set<uint64_t> page_set;
    uint64_t pg_count = 0;
    uint64_t count = 0;

    uint64_t limit = sorted_truth.size()/10;

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
    limit = sorted_ad.size()/10;

    for( auto i : sorted_ad) {

       if(count < limit) {
           if(page_set.find(i.first) != page_set.end()) {
             pg_count++;
           }
           count++;
       }
       else {
           break;
       }

    }

    fprintf(stdout, "ad could identify %f of the top 10%% pages\n",
    ((float) pg_count/limit));
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

void analyze_trace(bool gen_file_data, 
                    int64_t instr_limit = std::numeric_limits<int64_t>::max()) {
    std::unordered_map<uint64_t, uint64_t> page_to_count;
    std::unordered_map<uint64_t, uint64_t> pages_seen;

//     std::unordered_set<uint64_t> pages_seen;

    int64_t seen_instr = 0;
    int num_reads = 0;
    int64_t start_instr = 0;
    bool first = true;
    double time_stamp; 
    int num_iterations;

    while (instr_limit > seen_instr - start_instr) {
        num_reads ++;
        EntireEntry ee = trace.readNextEntry();
        if (!ee.valid) {
            break;
        }

        if (first) {
            start_instr = ee.eh.instructions;
            first = false;
            time_stamp = ee.eh.time;
        }

        seen_instr += ee.eh.instructions;

        if((ee.eh.time - time_stamp) >= (SAMPLE_TIME_IN_S * PIN_TIME_SCALE *
            MAX_ITERATIONS)) {

            sorted_ad = sort(pages_seen);
            sorted_truth = sort(page_to_count);

            checkSimilarity();


            // what do we want to analyze? we also have to clear pages_seen and
            // pages_to_count in here every 10s 

        }

        for (auto pe: ee.pe_list) {

            auto it = page_to_count.find(pe.page_num);
            if (it == page_to_count.end()) {
                page_to_count[pe.page_num] = pe.accesses;
            } else {
                page_to_count[pe.page_num] += pe.accesses;
            }
            total_truth += pe.accesses;

            it = pages_seen.find(pe.page_num);
            if (it == pages_seen.end()) {
                pages_seen[pe.page_num] = 1;
            } else {
//                 pages_seen[pe.page_num] += 1;
                pages_seen[pe.page_num]++;
                pages_seen[pe.page_num] %= 10;
            }
//             pages_seen.insert(pe.page_num);
        }

        
        if (num_reads % 1001 == 1000) {
            std::cerr << '.';
        }
    }

//     sorted_pebs = sort(pebs_count);
// 
//     for( auto it: sorted_pebs) {
//         total_pebs += it.second;
//     }
//     sorted_truth = sort(page_to_count);
// 
//    if(gen_file_data)  {
//        for(auto it: sorted_pebs) {
//            fptr.write((char*)&it, sizeof(it));
//        }
// 
//        std::pair<uint64_t, uint64_t> sep;
//        sep.first = 0;
//        sep.second = 0;
// 
//        fptr.write((char*)&sep, sizeof(sep));
// 
//        for(auto it: sorted_truth) {
//            fptr.write((char*)&it, sizeof(it));
//        }
// 
//     }

    std::cerr << std::endl;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "./pebs tracefile [ [-g] sorted_trace_file]" << std::endl;
        return 1;
    }
    bool use_file_data = false;
    bool gen_file_data = false;

    if( argc >= 3) {
        if( strcmp(argv[2], "-g") == 0) {
            if(argc > 4) {
                std::cerr << "./pebs tracefile [ [-g] sorted_trace_file]" << std::endl;
                return 1;
            } else {
                fptr.open(argv[3], std::fstream::in | std::fstream::out | std::fstream::trunc | std::ios::in | std::ios::out | std::ios::binary);
                std::cerr << "Error: " << strerror(errno) << " line 282" << std::endl;
                gen_file_data = true;
            }
        } else {
            fptr.open(argv[2], std::ios::in | std::ios::out | std::ios::binary);
            std::cerr << "Error: " << strerror(errno) << " line 288" << std::endl;
            
            use_file_data = true;
        }
    }

    trace.setTraceFile(argv[1]);

//     if(use_file_data) {
//         read_from_sorted();
//     } else {
//         analyze_trace(gen_file_data);
//     }

    analyze_trace(true);

    fptr.close();

//     checkSimilarity();

}

