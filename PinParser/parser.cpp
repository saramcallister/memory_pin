#include <cerrno>
#include <cstring>
#include <clocale>
#include <iostream>

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

void TraceFile::setTraceFile(const char* filename) {
    trace = fopen(filename, "r");
    if (trace == NULL) {
        std::cerr << "Failed to open trace file (" << filename << ") for reading"
            << " with error: " << std::strerror(errno) << std::endl;
        exit(1);
    }
}
