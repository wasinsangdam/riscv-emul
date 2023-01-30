#include <iostream>
#include <fstream>
#include <string>
#include "../inc/RiscV.h"
#include "../inc/parse.h"

using namespace std;

int main (int argc, char* argv[]) {
    bool* print_opt   = new bool;
    string* file_name = new string;

    arg_parse(argc, argv, file_name, print_opt);

    RiscV* riscv = new RiscV(file_name, print_opt);

    riscv->run();

    delete print_opt;
    delete file_name;

    return 0;
    
}
