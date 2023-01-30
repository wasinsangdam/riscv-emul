#include "../inc/Memory.h"
#include <fstream>
#include <iomanip>
#include <bitset>
#include <string>

// ----------------- MEMORY ------------------ //
Memory::Memory(std::string* file_name) {    
    std::ifstream file(*file_name, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open program file!\n");
    }

    file.read((char*)memory, DRAM_SIZE);
    file.close();
    
}
uint32_t Memory::load_8(uint32_t addr) {
    return (uint32_t) memory[addr - BASE_ADDR];
}

uint32_t Memory::load_16(uint32_t addr) {
    return (uint32_t) memory[addr - BASE_ADDR] 
        |  (uint32_t) memory[addr - BASE_ADDR + 1] << 8;
}

uint32_t Memory::load_32(uint32_t addr) {
    return (uint32_t) memory[addr - BASE_ADDR]
        |  (uint32_t) memory[addr - BASE_ADDR + 1] << 8
        |  (uint32_t) memory[addr - BASE_ADDR + 2] << 16
        |  (uint32_t) memory[addr - BASE_ADDR + 3] << 24;

}

uint32_t Memory::mem_load(uint32_t addr, uint32_t size) {
    switch (size) {
        case 8  : return load_8 (addr); break;
        case 16 : return load_16(addr); break;
        case 32 : return load_32(addr); break;

        default : {
            std::cout << size << " can't be size.\n";
            return 1;
        }
    }
}

void Memory::store_8(uint32_t addr, uint32_t data) {
    memory[addr - BASE_ADDR] = (uint8_t) (data & 0xFF);
}

void Memory::store_16(uint32_t addr, uint32_t data) {
    memory[addr - BASE_ADDR    ] = (uint8_t) (data & 0xFF);
    memory[addr - BASE_ADDR + 1] = (uint8_t) ((data >> 8) & 0xFF);
}

void Memory::store_32(uint32_t addr, uint32_t data) {
    memory[addr - BASE_ADDR    ] = (uint8_t) (data & 0xFF);
    memory[addr - BASE_ADDR + 1] = (uint8_t) ((data >> 8 ) & 0xFF);
    memory[addr - BASE_ADDR + 2] = (uint8_t) ((data >> 16) & 0xFF);
    memory[addr - BASE_ADDR + 3] = (uint8_t) ((data >> 24) & 0xFF);
}

void Memory::mem_store(uint32_t addr, uint32_t size, uint32_t data) {
    switch (size) {
        case 8  : store_8 (addr, data); break;
        case 16 : store_16(addr, data); break;
        case 32 : store_32(addr, data); break;

        default : {
            std::cout << size << " can't be size.\n";
            return;
        }
    }
}
// ----------------- MEMORY ------------------ //