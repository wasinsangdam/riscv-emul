#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdexcept>
#include <iostream>

void invalid_arg(std::string str);              // Error with invalid argument when execute main
void hard_x0();                                 // REG[0] (x0) must be zero

void inst_addr_misaligned(const int line);      // Invalid pc, (pc % 4 != 0)
void inst_access_fault(const int line);         // Out-of-memory access
void illegal_inst(const int line);              // Invalid operation code
void load_addr_misaligned(const int line);      // Invalid address (address % 4 != 0)
void store_addr_misaligned(const int line);     // Invalid address (address % 4 != 0)
void load_access_fault(const int line);         // Out-of-memory access
void store_access_fault(const int line);        // Out-of-memory access

#endif