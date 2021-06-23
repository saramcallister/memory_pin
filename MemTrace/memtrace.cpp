/*
 * Copyright 2002-2020 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
 */

/*
 *  This file contains an ISA-portable PIN tool for tracing memory accesses.
 */

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>

#include "pin.H"


FILE* trace;
std::ifstream msr_read;
std::ofstream msr_write;
clock_t start;
uint64_t start_msr;
int cpu;
uint64_t last_printed = 0;
uint64_t ACCESS_INTERVAL = 300000;

uint32_t MSR_REG = 777; // for fixed counter 0
uint32_t MSR_CTRL_REG = 911; // turn on counter
uint32_t MSR_FIXED_CTR_CTRL = 0x38d; // turn on fixed counter control
uint64_t original_fixed_ctr_ctrl = 0;
uint32_t CPUS = 40;

// page -> num accesses
std::unordered_map<uint64_t, uint32_t> pages;

uint64_t rdmsr_on_cpu() {
    uint64_t data;
    char msr_file_name[64];

    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    msr_read.open(msr_file_name);
    if (!msr_read) {
        fprintf(stderr, "rmsr: life be sad on open");
        exit(2);
    }
    
    msr_read.seekg(MSR_REG, msr_read.beg);
    msr_read.read((char *) &data, sizeof(data));
    msr_read.close();
    if (!msr_read) {
        fprintf(stderr, "rdmsr: pread");
        exit(4);
    }

    return data;
}

VOID start_recording_on_cpu() {
    uint64_t data = 0x3;
    char msr_file_name[64];
    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);

    msr_read.open(msr_file_name);
    if (!msr_read) {
        fprintf(stderr, "rdmsr: life be sad on open");
        exit(2);
    }
    msr_read.seekg(MSR_FIXED_CTR_CTRL, msr_read.beg);
    msr_read.read((char *) &original_fixed_ctr_ctrl, sizeof(original_fixed_ctr_ctrl));
    msr_read.close();
    if (!msr_read) {
        fprintf(stderr, "rdmsr: pread");
        exit(4);
    }
    //original_fixed_ctr_ctrl = 0xb0;

    msr_write.open(msr_file_name, std::ofstream::binary | std::ofstream::out);
    if (!msr_write) {
        fprintf(stderr, "wrmsr: life be sad on open");
        exit(2);
    }
    data = data | original_fixed_ctr_ctrl;
    //data = 0xb3;
    msr_write.seekp(MSR_FIXED_CTR_CTRL, msr_write.beg);
    //msr_write.seekp(MSR_CTRL_REG, msr_write.beg);
    //data = 0x1ULL << 32;
    uint8_t write_data = 0xb3;
    std::cout << msr_file_name << " " << MSR_FIXED_CTR_CTRL << " " << write_data << " " << sizeof(write_data) << std::endl;
    msr_write.write((char *) &write_data, sizeof(write_data));
    msr_write.close();
    if (!msr_write) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        fprintf(stderr, "start_recording_on_cpu: write");
        exit(4);
    }
}

VOID stop_recording_on_cpu() {
    char msr_file_name[64];
    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    msr_write.open(msr_file_name);
    if (!msr_write) {
        fprintf(stderr, "wrmsr: life be sad on open");
        exit(2);
    }

    msr_write.seekp(MSR_FIXED_CTR_CTRL, msr_write.beg);
    msr_write.write((char *) &original_fixed_ctr_ctrl, sizeof(original_fixed_ctr_ctrl));
    msr_write.close();
    if (!msr_write) {
        fprintf(stderr, "wrmsr: write");
        exit(4);
    }
}

// memory address -> page info (will create empty node if it doesn't exist)
VOID incPageAccesses(VOID *addr) {
    uint64_t p = (uint64_t) addr >> 12; // Assuming 4KB pages
    std::unordered_map<uint64_t, uint32_t>::iterator it;
    it = pages.find(p);
    if (it == pages.end()) {
        pages[p] = 1;
    } else {
        it->second++;
    }
}

double getTime()
{
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

struct EntryHeader {
    double time;
    uint64_t instructions;
    uint64_t num_pages;
};

struct PageEntry {
    uint64_t page_num;
    uint64_t accesses;
};

VOID printFormatted() {
    EntryHeader eh = {getTime(), rdmsr_on_cpu(), pages.size()};
    fwrite(&eh, sizeof(eh), 1, trace);

    PageEntry pe;
    for (std::unordered_map<uint64_t, uint32_t>::iterator it = pages.begin(); it != pages.end(); it++) {
        pe.page_num = it->first;
        pe.accesses = it->second;
        fwrite(&pe, sizeof(pe), 1, trace);
    }
    pages.clear();
}

/*VOID clearPagesAndPrint() {
    trace << "\"" << getTime() << ", " << rdmsr_on_cpu() - start_msr;
    trace << "\": [";
    for (std::unordered_map<uint64_t, uint32_t>::iterator it = pages.begin(); it != pages.end(); it++) {
        trace << "{\"" << it->first << "\": " << it->second << "}, ";
    }
    trace << "], \n";
    pages.clear();
}*/


/*VOID printTime()
{
    clock_t end = clock();
    double total_t = (double)(end - start) / CLOCKS_PER_SEC;
    fprintf(trace, "Seconds: %f\n", total_t);
}*/

// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr)
{
    if (!(last_printed % ACCESS_INTERVAL)) {
        printFormatted();
    }
    incPageAccesses(addr);
    last_printed++;
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
    if (!(last_printed % ACCESS_INTERVAL)) {
        printFormatted();
    }
    incPageAccesses(addr);
    last_printed++;
}

KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", 
        "memtrace.out", "specify output file name");

KNOB<int> KnobCPU(KNOB_MODE_WRITEONCE, "pintool", "p", 
        "0", "specify pinned cpu");

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP 
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_END);
        }
        // Note that in some architectures a single memory operand can be 
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_END);
        }
    }
}

VOID Fini(INT32 code, VOID *v)
{
    //fprintf(trace, "#eof\n");
    //fclose(trace);
    printFormatted();
    //stop_recording_on_cpu();
    fclose(trace);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
INT32 Usage()
{
    PIN_ERROR( "This Pintool prints a trace of memory addresses\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    if (PIN_Init(argc, argv)) return Usage();

    sleep(1);

    cpu = KnobCPU.Value();
    //start_recording_on_cpu();

    trace = fopen(KnobOutputFile.Value().c_str(), "w");
    
    start = clock();
    start_msr = rdmsr_on_cpu();

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
