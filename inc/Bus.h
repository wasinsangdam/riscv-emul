#ifndef __BUS_H__
#define __BUS_H__

#include <iostream>
#include "Memory.h"


// ----------------- BUS ------------------ //
class Bus { 
    Memory* bus_mem;    // For connect to Memory    

public:
    // Connect Bus to Mermoy
    Bus(Memory* memory);

    // Load data in Memory through Bus (BUS <- MEM)
    uint32_t bus_load(uint32_t addr, uint32_t size);  
    
    // Store data in Memory through Bus (BUS -> MEM)           
    void bus_store(uint32_t addr, uint32_t size, uint32_t data);
};
// ----------------- BUS ------------------ //

#endif