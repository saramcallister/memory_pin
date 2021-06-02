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
#include <map>
#include "pin.H"


std::ofstream trace;
clock_t start;
uint64_t last_printed = 0;
uint64_t ACCESS_INTERVAL = 300000;

// page -> num accesses
std::map<uint64_t, uint32_t> pages;

// memory address -> page info (will create empty node if it doesn't exist)
VOID incPageAccesses(VOID *addr) {
    uint64_t p = (uint64_t) addr >> 12; // Assuming 4KB pages
    std::map<uint64_t, uint32_t>::iterator it;
    it = pages.find(p);
    if (it == pages.end()) {
        pages[p] = 1;
    }
    it->second++;
}

double getTime()
{
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

VOID clearPagesAndPrint() {
    trace << "\"" << getTime() << "\": [";
    for (std::map<uint64_t, uint32_t>::iterator it = pages.begin(); it != pages.end(); it++) {
        trace << "{\"" << it->first << "\": " << it->second << "}, ";
    }
    trace << "], \n";
    pages.clear();
}


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
        clearPagesAndPrint();
    }
    incPageAccesses(addr);
    last_printed++;
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
    if (!(last_printed % ACCESS_INTERVAL)) {
        clearPagesAndPrint();
    }
    incPageAccesses(addr);
    last_printed++;
}

KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", 
        "memtrace.out", "specify output file name");

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
    clearPagesAndPrint();
    trace << "}";
    trace.close();
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

    trace.open(KnobOutputFile.Value().c_str());
    trace << "{\n";
    
    start = clock();

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
