#include "../inc/error.h"

// Error with invalid argument when execute main
void invalid_arg(std::string str) {
    throw std::invalid_argument(str);
}

// REG[0] (x0) must be zero
void hard_x0() {
    throw std::runtime_error("REG[0] (x0) is hardwired register.");
}

// Invalid pc, (pc % 4 != 0)
void inst_addr_misaligned(const int line) {
    std::cout << "Line : " << line << "\n";
    throw std::runtime_error("Invalid PC(instruction address).\n");
}

// Out-of-memory access
void inst_access_fault(const int line) {
    std::cout << "Line : " << line << "\n";
    throw std::out_of_range("Out-of-memory PC(instruction addeess).\n");
}   

// Invalid operation code
void illegal_inst(const int line) {
    std::cout << "Line : " << line << "\n";
    throw std::runtime_error("Invalid operation code.\n");
}   

// Invalid load address (address % 4 != 0)
void load_addr_misaligned(const int line) {
    std::cout << "Line : " << line << "\n";
    throw std::runtime_error("Invalid load address.\n");
}   
// Invalid store address (address % 4 != 0)
void store_addr_misaligned(const int line) {
    std::cout << "Line : " << line << "\n";
    throw std::runtime_error("Invalid store address.\n");
}

// Out-of-memory access
void load_access_fault(const int line) {
    std::cout << "Line : " << line << "\n";
    throw std::out_of_range("Out-of-memory load address.\n");
}       

// Out-of-memory access
void store_access_fault(const int line) {
    std::cout << "Line : " << line << "\n";
    throw std::out_of_range("Out-of-memory store address.\n");
}       



