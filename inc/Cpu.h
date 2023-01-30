#ifndef __CPU_H__
#define __CPU_H__

#include <iostream>
#include <iomanip>
#include <string>
#include <bitset>
#include "Bus.h"
#include "error.h"
#include "opcodes.h"

// ----------------- CPU ------------------ //
class Cpu {
    
    Bus* cpu_bus;             // For connect cpu to bus
    bool print_flag;          // Determine whether to print regsiters, pc and instructions
    uint32_t regs[32] = {};   // RISC-V32 has 32 32bits registers

    uint32_t csrs[4096] = {}; // For Zicsr Extension (Control and Status Register)
    
    // Load data in Cpu through Bus (Cpu <- Bus <- Mem)
    uint32_t cpu_load(uint32_t addr, uint32_t size);

    // Store data in Mem through Bus (CPU -> BUS -> MEM)
    void cpu_store(uint32_t addr, uint32_t size, uint32_t data);

    // Load CSR register
    uint32_t csr_load(uint32_t csr);
    // Store CSR register
    void csr_store(uint32_t csr, uint32_t value);

    void print_reg();
    void print_R(const char* inst, uint32_t rd, uint32_t rs1, uint32_t rs2);
    void print_I(const char* inst, uint32_t rd, uint32_t rs1, uint32_t immd);
    void print_S(const char* inst, uint32_t rs1, uint32_t rs2, uint32_t immd);
    void print_B(const char* inst, uint32_t rs1, uint32_t rs2, uint32_t immd);
    void print_U(const char* inst, uint32_t rd, uint32_t immd);
    void print_J(const char* inst, uint32_t rd, uint32_t immd);
    void print_CSR(const char* inst, uint32_t rd, uint32_t rs1, uint32_t csr);
    void print_SYS(const char* inst);

    // Decode
    // Instruction decode
    uint32_t get_opcode(uint32_t inst);     // Get opcode in instruction
    uint32_t get_rd (uint32_t inst);        // Get rd in instruction
    uint32_t get_rs1(uint32_t inst);        // Get rs1 in instruction
    uint32_t get_rs2(uint32_t inst);        // Get rs2 in instruction
    uint32_t get_funct3(uint32_t inst);     // Get funct3 in instruction
    uint32_t get_funct7(uint32_t inst);     // Get funct7 in instruction
    uint32_t get_shamt5(uint32_t inst);     // Get shamt7 in instrucion
    uint32_t get_immd_I(uint32_t inst);     // Get immd_I in instruction
    uint32_t get_immd_S(uint32_t inst);     // Get immd_S in instruction
    uint32_t get_immd_B(uint32_t inst);     // Get immd_B in instruction
    uint32_t get_immd_U(uint32_t inst);     // Get immd_U in instruction
    uint32_t get_immd_J(uint32_t inst);     // Get immd_J in instruction
    uint32_t get_csr(uint32_t inst);        // Get csr in instruction

    // Execution
    // R-TYPE (rd, funct3, rs1, rs2, funct7)
    // - RV32I (Integer Register-Register Operation)
    void exec_ADD(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_SUB(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_SLL(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_SLT(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_SLTU(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_XOR(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_SRL(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_SRA(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_OR(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_AND(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    // - RV32M Standard Extension(Integer Register-Register Operation)
    void exec_MUL(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_MULH(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_MULHSU(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_MULHU(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_DIV(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_DIVU(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_REM(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);
    void exec_REMU(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7);

    // I-TYPE (rd, funct3, rs1, immd) 
    //        (rd, funct3, rs1, immd, shamt5) only SLLI, SRLI, SRAI
    // - JALR
    void exec_JALR(uint32_t rd, uint32_t rs1, uint32_t immd);
    // - Load Instructions
    void exec_LB(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_LH(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_LW(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_LBU(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_LHU(uint32_t rd, uint32_t rs1, uint32_t immd);
    // - Integer Register-Immediate Operations
    void exec_ADDI(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_SLTI(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_SLTIU(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_XORI(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_ORI(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_ANDI(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_SLLI(uint32_t rd, uint32_t rs1, uint32_t immd, uint32_t shamt5);
    void exec_SRLI(uint32_t rd, uint32_t rs1, uint32_t immd, uint32_t shamt5);
    void exec_SRAI(uint32_t rd, uint32_t rs1, uint32_t immd, uint32_t shamt5);
    // - FENCE
    void exec_FENCE(uint32_t rd, uint32_t rs1, uint32_t immd);
    // - SYSTEM instructions
    void exec_ECALL(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_EBREAK(uint32_t rd, uint32_t rs1, uint32_t immd);
    void exec_MRET(uint32_t rd, uint32_t rs1, uint32_t immd);
    // - Zicsr Standard Extension
    void exec_CSRRW(uint32_t rd, uint32_t rs1, uint32_t csr);
    void exec_CSRRS(uint32_t rd, uint32_t rs1, uint32_t csr);
    void exec_CSRRC(uint32_t rd, uint32_t rs1, uint32_t csr);
    void exec_CSRRWI(uint32_t rd, uint32_t rs1, uint32_t csr);
    void exec_CSRRSI(uint32_t rd, uint32_t rs1, uint32_t csr);
    void exec_CSRRCI(uint32_t rd, uint32_t rs1, uint32_t csr);

    // S-TYPE (immd, funct3, rs1, rs2)
    // - Store intructions
    void exec_SB(uint32_t immd, uint32_t rs1, uint32_t rs2);
    void exec_SH(uint32_t immd, uint32_t rs1, uint32_t rs2);
    void exec_SW(uint32_t immd, uint32_t rs1, uint32_t rs2);

    // B-TYPE (immd, funct3, rs1, rs2)
    // - Branch instructions
    void exec_BEQ(uint32_t immd, uint32_t rs1, uint32_t rs2);
    void exec_BNE(uint32_t immd, uint32_t rs1, uint32_t rs2);
    void exec_BLT(uint32_t immd, uint32_t rs1, uint32_t rs2);
    void exec_BGE(uint32_t immd, uint32_t rs1, uint32_t rs2);
    void exec_BLTU(uint32_t immd, uint32_t rs1, uint32_t rs2);
    void exec_BGEU(uint32_t immd, uint32_t rs1, uint32_t rs2);

    // U-TYPE (rd, immd)
    // - LUI, AUIPC
    void exec_LUI(uint32_t rd, uint32_t immd);
    void exec_AUIPC(uint32_t rd, uint32_t immd);

    // J-TYPE (rd, immd)
    // - JAL
    void exec_JAL(uint32_t rd, uint32_t immd);

public:
    Cpu(Bus* bus, bool* print_opt);       // Connect Cpu to Bus, Set print option
    uint32_t pc;                          // Program Counter
    uint32_t next_pc;                     // next Program Counter
    uint32_t inst_fetch();                // Fetch instruction from memory
    void inst_decode_exec(uint32_t inst); // Decode the instruction + execute instruction
                                          
    bool check_reg0();                    // Check x0(REG[0]) is zero
};
// ----------------- CPU ------------------ //

#endif
