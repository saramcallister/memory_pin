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
#define TRACEFILE_INDEX 1 
#define PERCENT_HOT_INDEX 2 
#define GEN_FLAG_INDEX 3
#define ARGV_SORTED_FILE_LEN 4
#define GEN_FILE_READ_INDEX 3
#define GEN_FILE_WRITE_INDEX 4
#define MAX_ARGV_LEN 5

std::vector<std::pair<uint64_t, uint64_t>> sorted_pebs;
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

void read_from_sorted() {
    std::pair<uint64_t, uint64_t> read;
    bool read_pebs = true;

    fptr.seekg(0, fptr.beg);

    while(fptr) {
        fptr.read((char *)&read, sizeof(read));
        if(fptr) {
            if(read.second == 0) {
                read_pebs = false;
                continue;
            }
            if(read_pebs) {
                sorted_pebs.push_back(read);
            } else {
                sorted_truth.push_back(read);
            }
        }
    }

    for( auto it : sorted_pebs) {
        total_pebs += it.second;
    }
    for( auto it : sorted_truth) {
        total_truth += it.second;
    }

}

void compare() {

    for (auto it: truth_read) {
        auto found = std::find(sorted_truth.begin(), sorted_truth.end(),
        it);

        if( found == sorted_truth.end() || it.second != found->second) {
            fprintf(stderr, "read and sorted is not the same\n");
            exit(1);
        }
    }

    for (auto it: pebs_read) {
        auto found = std::find(sorted_pebs.begin(), sorted_pebs.end(),
        it);

        if( found == sorted_pebs.end() || it.second != found->second) {
            fprintf(stderr, "read and sorted is not the same\n");
            exit(1);
        }
    }

}

void checkSimilarity() {

    std::unordered_set<uint64_t> page_set;
    uint64_t pg_count = 0;
    uint64_t count = 0;

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
    limit = sorted_pebs.size()*percent_hot_pages;

    for( auto i : sorted_pebs) {

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

    fprintf(stdout, "pebs could identify %f of the top %f%% pages\n",
    ((float) pg_count/limit), percent_hot_pages*100);
    fprintf(stdout, 
        "pg_count: %ld, sorted_truth.size(): %ld, sorted_pebs.size(): %ld\n", 
            pg_count, sorted_truth.size(), sorted_pebs.size());

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
    std::unordered_map<uint64_t, uint64_t> pebs_count;

    int64_t seen_instr = 0;
    int num_reads = 0;
    int64_t start_instr = 0;
    bool first = true;
    uint64_t access_counter=0;

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
//             access_counter += pe.accesses;

//             if( access_counter > PEBS_FREQ) {
//                 auto it = pebs_count.find(pe.page_num);
// 
//                 if( it == pebs_count.end()) {
//                     pebs_count[pe.page_num] = 1;
//                 } else {
//                     it->second++;
//                 }
                

//                 access_counter -= PEBS_FREQ;
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
                        it->second++;
                    }
                    break;
                }
                
            }

        }

        access_counter += tot_accesses;
        access_counter = access_counter % PEBS_FREQ;

        if (num_reads % 1001 == 1000) {
            std::cerr << '.';
        }
    }

    sorted_pebs = sort(pebs_count);

    for( auto it: sorted_pebs) {
        total_pebs += it.second;
    }
    sorted_truth = sort(page_to_count);

   if(gen_file_data)  {
       for(auto it: sorted_pebs) {
           fptr.write((char*)&it, sizeof(it));
       }

       std::pair<uint64_t, uint64_t> sep;
       sep.first = 0;
       sep.second = 0;

       fptr.write((char*)&sep, sizeof(sep));

       for(auto it: sorted_truth) {
           fptr.write((char*)&it, sizeof(it));
       }

    }

    std::cerr << std::endl;
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "./pebs tracefile percent_hot_pages [ [-g] sorted_trace_file]" << std::endl;
        return 1;
    }
    bool use_file_data = false;
    bool gen_file_data = false;
    float percent_input = std::stoi(argv[PERCENT_HOT_INDEX]);

    percent_hot_pages = percent_input/100;

    

    if( argc >= ARGV_SORTED_FILE_LEN ) {
        if( strcmp(argv[GEN_FLAG_INDEX], "-g") == 0) {
            if(argc > MAX_ARGV_LEN) {
                std::cerr << "./pebs tracefile percent_hot_pages [ [-g] sorted_trace_file]" << std::endl;
                return 1;
            } else {
                errno = 0;
                fptr.open(argv[GEN_FILE_WRITE_INDEX], std::fstream::in | std::fstream::out | std::fstream::trunc | std::ios::in | std::ios::out | std::ios::binary);

                if(errno) {
                    std::cerr << "Error: " << strerror(errno) << " line 282" << std::endl;
                }
                gen_file_data = true;
            }
        } else {
            errno = 0;
            fptr.open(argv[GEN_FILE_READ_INDEX], std::ios::in | std::ios::out | std::ios::binary);

            if(errno) {
                std::cerr << "Error: " << strerror(errno) << " line 288" << std::endl;
            }
            
            use_file_data = true;
        }
    }

    trace.setTraceFile(argv[TRACEFILE_INDEX]);

    if(use_file_data) {
        read_from_sorted();
    } else {
        analyze_trace(gen_file_data);
    }

    fptr.close();

    checkSimilarity();

}

