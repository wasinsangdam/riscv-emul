#include "../inc/RiscV.h"

RiscV::RiscV(std::string* file_name, bool* print_opt) {
    memory = new Memory(file_name);     // Init memory with binary file
    bus = new Bus(memory);              // Connect Bus to Memory
    cpu = new Cpu(bus, print_opt);      // Connect Cpu to Bus

    if (*print_opt) print_flag = true;  // Set print option
    else            print_flag = false;

    file = *file_name;
}   

RiscV::~RiscV() {
    delete memory;
    delete bus;
    delete cpu;
}

// Print PC and instruction with hex
void RiscV::print_pc_inst(uint32_t inst) {
    if (print_flag) {
        std::cout << "PC : 0x"   << std::setw(8) << std::setfill('0') << std::hex << cpu->pc << "  \t";
        std::cout << "INST : 0x" << std::setw(8) << std::setfill('0') << std::hex << inst    << "  \n";
        std::cout << std::dec;
    }
}

// Extract only file name without directory
std::string RiscV::only_file(std::string file) {
    std::string::size_type n;

    n = file.find_last_of("/") + 1;
    file = file.substr(n);

    n = file.find(".bin");
    file = file.substr(0, n);

    return file;
}

// Check whether the test was successful
bool RiscV::is_success(uint32_t inst) {
    // Termination condition
    // 0xc0001073 has riscv-test repository set the [unimp] command to RVTEST_CODE_END macro
    if (inst == 0 || cpu->pc == 0 || inst == 0xc0001073)
        return true;
    else
        return false;
}

// Check <fail> function address in dump file
uint32_t RiscV::get_fail_pc(std::string file) {

    FILE *fp = NULL;

    std::string rv32ui = "./dump/rv32ui-p-";
    std::string rv32um = "./dump/rv32um-p-";
    std::string ext    = ".dump";

    std::string dump_file = "";
    
    if (file.find("div") != std::string::npos || file.find("mul") != std::string::npos || file.find("rem") != std::string::npos)
        dump_file.append(rv32um + file + ext);
    else
        dump_file.append(rv32ui + file + ext);

    std::string command = "cat " + dump_file + " | grep -i \"<fail>:\" | cut -c 1-8";

    const char *command_char = command.c_str();

    char char_fail_pc[8];

    if ((fp = popen(command_char, "r")) == NULL) {
        return 1;
    }

    int status = fread(char_fail_pc, sizeof(char), sizeof(char_fail_pc), fp);

    if (status != sizeof(char_fail_pc)) {
        std::cout << "Reading Error!\n";
        exit(0);
    }

    pclose(fp);

    uint32_t fail_pc = (uint32_t)strtol(char_fail_pc, NULL, 16);

    return fail_pc;
}

void RiscV::run() {

    uint32_t fail_pc = get_fail_pc(only_file(file));

    while (1) {

        cpu->next_pc  = cpu->pc + 4;

        uint32_t inst = cpu->inst_fetch();

        print_pc_inst(inst);

        cpu->inst_decode_exec(inst);


        // When PC meets <fail> function address
        if (cpu->pc == fail_pc) {
            std::cout << "[" << only_file(file) << "] " << std::setw(13) << "\t" << std::left << "\tTEST FAILED \n";
            break;
        }

        // When test exits with success
        if (is_success(inst)) {
            std::cout  << "[" << only_file(file) << "] "<< std::setw(13) << "\t" <<std::left << "\tTEST PASSED \n";
            break;
        }

        // Always make sure that x0 remains at zero 
        if (!cpu->check_reg0()) {
            hard_x0();
        }

        cpu->pc = cpu->next_pc;
    }
}