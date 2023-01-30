#include "../inc/error.h"

void invalid_arg(std::string str) {
    throw std::invalid_argument(str);
}

void invalid_opcode(std::string str, const int line) {
    std::cout << "Line : " << std::dec << line << "\n";
    throw std::runtime_error(str);
}