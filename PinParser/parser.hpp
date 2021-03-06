#include <cstdint>
#include <stdint.h>
#include <string>
#include <unistd.h>
#include <vector>

struct EntryHeader {
    double time;
    uint64_t instructions;
    uint64_t num_pages;
};

struct PageEntry {
    uint64_t page_num;
    uint64_t accesses;

    PageEntry(uint64_t pn, uint64_t a) {
        page_num = pn;
        accesses = a;
    }

    PageEntry() {}
};

class PageEntryComparator {
public:
    int operator() (const PageEntry& pe1, const PageEntry& pe2) {
        return pe1.accesses < pe2.accesses;
    }
};

struct EntireEntry {
    EntryHeader eh;
    std::vector<PageEntry> pe_list;
    bool valid; // set to false at EOF
};

class TraceFile {
    public:
        TraceFile() = default;
        ~TraceFile() {
            if (trace) {
                fclose(trace);
            }
        };
        EntireEntry readNextEntry();
        void setTraceFile(const char* filename); // can cause program to exit

    private:
        FILE* trace;
};
