#ifndef __RISC_V_H__
#define __RISC_V_H__

#include <iostream>
#include <string>
#include <iomanip>
#include <bitset>

#include "Cpu.h"
#include "Bus.h"
#include "Memory.h"

// ---------------- Risc-V ---------------- //
// ----------- Cpu + Bus + Memory --------- //
class RiscV {
    Cpu*    cpu;
    Memory* memory;
    Bus*    bus;

    bool print_flag;
    std::string file;

    void print_pc_inst(uint32_t inst);         // Print PC and instruction with hex
    std::string only_file(std::string file);   // Extract only file name witout directory

    bool is_success(uint32_t inst);            // Check whether the test was successful

public:
    RiscV(std::string* file_name, bool* print_opt);
    ~RiscV();
    void run();
};

// ---------------- Risc-V ---------------- //
// ----------- Cpu + Bus + Memory --------- //

#endif