#include <algorithm>
#include <cstdint>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parser.hpp"

TraceFile trace1;
TraceFile trace2;
FILE* outfile;

void compare(EntireEntry ee1, EntireEntry ee2, bool print) {
    uint64_t only1 = 0;
    uint64_t only2 = 0;
    uint64_t same = 0;

    std::set<uint64_t> pages1;
    std::set<uint64_t> pages2;
    for (auto pe: ee1.pe_list) {
        pages1.insert(pe.page_num);
    }
    for (auto pe: ee2.pe_list) {
        pages2.insert(pe.page_num);
    }
    
    //std::unoredered_set<uint64_t> result;
    if (print) {
        std::cout << "File 1 only: ";
    }
    std::vector<uint64_t> elems1;
    std::vector<uint64_t> elems2;
    for (auto elem: pages1) {
        if (pages2.find(elem) != pages2.end()) {
            same++;
        } else {
            if (print) {
                fprintf(stdout, " %lx", elem);
            }
            only1++;
            elems1.push_back(elem);
        }
    }
    if (print) {
        std::cout << "\nFile 2 only: ";
    }
    for (auto elem: pages2) {
        if (pages1.find(elem) != pages1.end()) {
            same++;
        } else {
            if (print) {
                fprintf(stdout, " %lx", elem);
            }
            only2++;
            elems2.push_back(elem);
        }
    }
    
    if (print) {
        std::cout << std::endl << "Differences: ";
        for (size_t i = 0; i < std::min(elems1.size(), elems2.size()); i++) {
            fprintf(stdout, " %lx ", elems2[i] - elems1[i]);
        }
        std::cout << std::endl;
    }

    std::cout << "only 1: " << only1 << " , only 2: " << only2 << " , same: " << same / 2 << std::endl;
    std::cout << "len of vecs " << ee1.pe_list.size() << " " << ee2.pe_list.size() << std::endl;
}

bool runNextComparison(bool print = false) {
    EntireEntry ee1 = trace1.readNextEntry();
    EntireEntry ee2 = trace2.readNextEntry();
    if (!ee1.valid || !ee2.valid) {
        return false;
    } 
    compare(ee1, ee2, print);
    return true;
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "./first_seen trace1 trace2" << std::endl;
        return 1;
    }

    trace1.setTraceFile(argv[1]);
    trace2.setTraceFile(argv[2]);

    for (int i = 0; i < 20; i++) {
        if (i % 10 == 9) {
            runNextComparison(true);
        }
        else {
            runNextComparison();
        }
    }
}


