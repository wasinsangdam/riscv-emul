#include <iostream>
#include <string>
#include "../inc/RiscV.h"
#include "../inc/parse.h"

using namespace std;

int main (int argc, char* argv[]) {
    bool    print_opt = false;
    string  file_name;

    arg_parse(argc, argv, &file_name, &print_opt);    // Parse arguments

    RiscV* riscv = new RiscV(&file_name, &print_opt); // Create RISC-V emulator with binary file and print option 

    riscv->run();

    delete riscv;

    return 0;
    
}
