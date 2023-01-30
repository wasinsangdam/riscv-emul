#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <iostream>
#define DRAM_SIZE 0x100000         // 1 MiB
#define BASE_ADDR 0x80000000       // Base Address
#define XLEN      32               // 4 B    

// ----------------- MEMORY ------------------ //
class Memory {
    // DRAM size with 64KiB
    uint8_t memory[DRAM_SIZE] = {};

    // Load Byte(8 bits), Half-Word(16 bits), Word(32 bits)
    uint32_t load_8 (uint32_t addr);
    uint32_t load_16(uint32_t addr);
    uint32_t load_32(uint32_t addr);

    // Stroe Byte(8 bits), Half-Word(16 bits), Word(32 bits)
    void store_8 (uint32_t addr, uint32_t data);
    void store_16(uint32_t addr, uint32_t data);
    void store_32(uint32_t addr, uint32_t data);

public:
    // Memory init
    Memory(std::string* file_name);
    
    // Load data with size at Memory addr
    uint32_t mem_load(uint32_t addr, uint32_t size);
    
    // Store data at Memory addr with size 
    void mem_store(uint32_t addr, uint32_t size, uint32_t data);

};
// ----------------- MEMORY ------------------ //

#endif