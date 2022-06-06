#include <cerrno>
#include <cstring>
#include <clocale>
#include <iostream>
#include <string>

#include "parser.hpp"

EntireEntry TraceFile::readNextEntry() {
    EntireEntry ee;
    ee.valid = true;
    size_t ret = fread(&ee.eh, sizeof(ee.eh), 1, trace);
    if (ret != 1) {
        ee.valid = false;
        return ee;
    }

    if (ee.eh.num_pages == 0) {
        return ee;
    }

    PageEntry pe;
    std::vector<PageEntry> pe_list;
    ee.pe_list.reserve(ee.eh.num_pages);
    for (uint64_t i = 0; i < ee.eh.num_pages; i++) {
        ret = fread(&pe, sizeof(pe), 1, trace);
        if (ret != 1) {
            ee.valid = false;
            return ee;
        }
        ee.pe_list.push_back(pe);
    }
    return ee;
}

int TraceFile::writeEntry(EntireEntry ee) {
    size_t ret = fwrite(&ee.eh, sizeof(ee.eh), 1, trace);
    if (ret != 1) {
        fprintf(stderr, "%d %s\n", ret, strerror(errno));
        return -1;
    }

    for(auto pe: ee.pe_list) {
        ret = fwrite(&pe, sizeof(pe), 1, trace);
//         fprintf(stdout, "%lu, %lu\n", pe.page_num, pe.accesses);
        if (ret != 1) {
            return -1;
        }
    }
    return 0;
}

void TraceFile::setTraceFile(const char* filename, bool ro) {
    if(ro) {
        trace = fopen(filename, "r");
    } else {
        trace = fopen(filename, "w");
    }
    if (trace == NULL) {
        std::cerr << "Failed to open trace file (" << filename << ") for reading"
            << " with error: " << std::strerror(errno) << std::endl;
        exit(1);
    }
}

void TraceFile::setTraceFile(const char* filename) {
    setTraceFile(filename, true);
}
