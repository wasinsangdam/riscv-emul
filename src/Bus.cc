#include "../inc/Bus.h"

// ----------------- BUS ------------------ //
Bus::Bus(Memory* memory) {
    bus_mem = memory;       // Connect Bus with Memory
}

uint32_t Bus::bus_load(uint32_t addr, uint32_t size) {
    return bus_mem->mem_load(addr, size);
}

void Bus::bus_store(uint32_t addr, uint32_t size, uint32_t data) {
    bus_mem->mem_store(addr, size, data);
}
// ----------------- BUS ------------------ //