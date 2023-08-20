#include "../inc/Cpu.h"

// ----------------- CPU ------------------ //
Cpu::Cpu(Bus* bus, bool* print_opt) {    
    if (*print_opt)  print_flag = true;
    else             print_flag = false;

    regs[0] = 0x0;                      // Hardwired zero register
    regs[2] = BASE_ADDR + DRAM_SIZE;    // Stack pointer register
    pc      = BASE_ADDR;                // Current instruction address

    // Connect Cpu to Bus
    cpu_bus = bus;
}

// Fetch instruction from memory (XLEN = 32 bits)
uint32_t Cpu::inst_fetch() {
    if (pc % 4 != 0) 
        inst_addr_misaligned(__LINE__);
    
    if ((pc < BASE_ADDR) && (pc > DRAM_SIZE + BASE_ADDR))
        inst_access_fault(__LINE__);

    return cpu_bus->bus_load(pc, XLEN);
} 

// Decode instruction + execute instruction
void Cpu::inst_decode_exec(uint32_t inst) {
    uint32_t opcode = get_opcode(inst);

    switch (opcode) {
        // R-TYPE
        case REG_REG : {
            uint32_t rd     = get_rd(inst);
            uint32_t funct3 = get_funct3(inst);
            uint32_t rs1    = get_rs1(inst);
            uint32_t rs2    = get_rs2(inst);
            uint32_t funct7 = get_funct7(inst);

            switch (funct7) {
                case RV32I1 : { // funct7 0b0000000
                    switch (funct3) {
                        case ADD  : exec_ADD(rd, rs1, rs2, funct7);  break;
                        case SLL  : exec_SLL(rd, rs1, rs2, funct7);  break;
                        case SLT  : exec_SLT(rd, rs1, rs2, funct7);  break;
                        case SLTU : exec_SLTU(rd, rs1, rs2, funct7); break;
                        case XOR  : exec_XOR(rd, rs1, rs2, funct7);  break;
                        case SRL  : exec_SRL(rd, rs1, rs2, funct7);  break;
                        case OR   : exec_OR(rd, rs1, rs2, funct7);   break;
                        case AND  : exec_AND(rd, rs1, rs2, funct7);  break;

                        default : illegal_inst(__LINE__);
                    }
                    break;
                }
                case RV32I2 : { // funct7 0b0100000
                    switch (funct3) {
                        case SUB : exec_SUB(rd, rs1, rs2, funct7); break;
                        case SRA : exec_SRA(rd, rs1, rs2, funct7); break;

                        default : illegal_inst(__LINE__); 
                    }
                    break;
                }
                case RV32M : { // funct7 0b0000001
                    switch (funct3) {
                        case MUL    : exec_MUL(rd, rs1, rs2, funct7);    break;
                        case MULH   : exec_MULH(rd, rs1, rs2, funct7);   break;
                        case MULHSU : exec_MULHSU(rd, rs1, rs2, funct7); break;
                        case MULHU  : exec_MULHU(rd, rs1, rs2, funct7);  break;
                        case DIV    : exec_DIV(rd, rs1, rs2, funct7);    break;
                        case DIVU   : exec_DIVU(rd, rs1, rs2, funct7);   break;
                        case REM    : exec_REM(rd, rs1, rs2, funct7);    break;
                        case REMU   : exec_REMU(rd, rs1, rs2, funct7);   break;

                        default : illegal_inst(__LINE__);
                    }
                    break;
                }
                
                default : illegal_inst(__LINE__);
            }
            break;
        }

        // I-TYPE
        case JALR : {
            uint32_t rd   = get_rd(inst);
            uint32_t rs1  = get_rs1(inst);
            uint32_t immd = get_immd_I(inst);

            exec_JALR(rd, rs1, immd); break;
        }

        case LOAD : {
            uint32_t rd     = get_rd(inst);
            uint32_t rs1    = get_rs1(inst);
            uint32_t funct3 = get_funct3(inst);
            uint32_t immd   = get_immd_I(inst);

            switch (funct3) {
                case LB  : exec_LB(rd, rs1, immd);  break;
                case LH  : exec_LH(rd, rs1, immd);  break;
                case LW  : exec_LW(rd, rs1, immd);  break;
                case LBU : exec_LBU(rd, rs1, immd); break;
                case LHU : exec_LHU(rd, rs1, immd); break;
                
                default : illegal_inst(__LINE__);   break;
            }
            break;
        }

        case REG_IMM : {
            uint32_t rd     = get_rd(inst);
            uint32_t rs1    = get_rs1(inst);
            uint32_t funct3 = get_funct3(inst);
            uint32_t immd   = get_immd_I(inst);
            uint32_t shamt5 = get_shamt5(inst); // For only SLLI, SRLI, SRAI
            uint32_t funct7 = get_funct7(inst); // For only SRLI, SRAI

            switch (funct3) {
                case ADDI  : exec_ADDI(rd, rs1, immd);          break;
                case SLTI  : exec_SLTI(rd, rs1, immd);          break;
                case SLTIU : exec_SLTIU(rd, rs1, immd);         break;
                case XORI  : exec_XORI(rd, rs1, immd);          break;
                case ORI   : exec_ORI(rd, rs1, immd);           break;
                case ANDI  : exec_ANDI(rd, rs1, immd);          break;
                case SLLI  : exec_SLLI(rd, rs1, immd, shamt5);  break;
                case SRI   : {
                    switch (funct7) {
                        case SRLI : exec_SRLI(rd, rs1, immd, shamt5); break;
                        case SRAI : exec_SRAI(rd, rs1, immd, shamt5); break;
                        default : illegal_inst(__LINE__); break;
                    }
                    break;
                }

                default : illegal_inst(__LINE__); break;
            }
            break;
        } 
        
        case FENCE : {
            uint32_t rd   = get_rd(inst);
            uint32_t rs1  = get_rs1(inst);
            uint32_t immd = get_immd_I(inst);

            exec_FENCE(rd, rs1, immd); break;
        }
        
        case SYSTEM : {
            uint32_t rd     = get_rd(inst);
            uint32_t rs1    = get_rs1(inst);
            uint32_t funct3 = get_funct3(inst);
            uint32_t immd   = get_immd_I(inst);
            uint32_t csr    = get_csr(inst);
            
            switch (funct3) {
                case CB : {
                    switch (immd) {
                        case ECALL  : exec_ECALL(rd, rs1, immd);  break;
                        case EBREAK : exec_EBREAK(rd, rs1, immd); break;
                        case MRET   : exec_MRET(rd, rs1, immd);   break;
        
                        default : illegal_inst(__LINE__); break;
                    }
                    break;
                }
                case CSRRW  : exec_CSRRW(rd, rs1, csr);  break;
                case CSRRS  : exec_CSRRS(rd, rs1, csr);  break;
                case CSRRC  : exec_CSRRC(rd, rs1, csr);  break;
                case CSRRWI : exec_CSRRWI(rd, rs1, csr); break;
                case CSRRSI : exec_CSRRSI(rd, rs1, csr); break;
                case CSRRCI : exec_CSRRCI(rd, rs1, csr); break;

                default : illegal_inst(__LINE__); break;
            }
            break;
        }
        
        // S-TYPE
        case (STORE) : {
            uint32_t immd   = get_immd_S(inst);
            uint32_t funct3 = get_funct3(inst);
            uint32_t rs1    = get_rs1(inst);
            uint32_t rs2    = get_rs2(inst);

            switch (funct3) {
                case SB : exec_SB(immd, rs1, rs2); break;
                case SH : exec_SH(immd, rs1, rs2); break;
                case SW : exec_SW(immd, rs1, rs2); break;

                default : illegal_inst(__LINE__);  break;
            }
            break;
        }

        // B-TYPE
        case (BRANCH) : {
            uint32_t immd   = get_immd_B(inst);
            uint32_t funct3 = get_funct3(inst);
            uint32_t rs1    = get_rs1(inst);
            uint32_t rs2    = get_rs2(inst);
            
            switch (funct3) {
                case BEG  : exec_BEQ(immd, rs1, rs2);  break;
                case BNE  : exec_BNE(immd, rs1, rs2);  break;
                case BLT  : exec_BLT(immd, rs1, rs2);  break;
                case BGE  : exec_BGE(immd, rs1, rs2);  break;
                case BLTU : exec_BLTU(immd, rs1, rs2); break;
                case BGEU : exec_BGEU(immd, rs1, rs2); break;

                default : illegal_inst(__LINE__); break;  
            }
            break;
        }

        // U-TYPE
        case LUI   :  {
            uint32_t rd   = get_rd(inst);
            uint32_t immd = get_immd_U(inst);
            exec_LUI(rd, immd);     break;
        }
        case AUIPC : {
            uint32_t rd   = get_rd(inst);
            uint32_t immd = get_immd_U(inst);
            exec_AUIPC(rd, immd);   break;
        }

        // J-TYPE
        case (JAL) : {
            uint32_t rd   = get_rd(inst);
            uint32_t immd = get_immd_J(inst);

            exec_JAL(rd, immd);     break; 
        }

        default : illegal_inst(__LINE__); break;
    }

    print_reg();
}

// Load data in Cpu through Bus (Cpu <- Bus <- Mem)
uint32_t Cpu::cpu_load(uint32_t addr, uint32_t size) {
    return cpu_bus->bus_load(addr, size);
}

// Store data in Mem through Bus (Cpu -> Bus -> Mem)
void Cpu::cpu_store(uint32_t addr, uint32_t size, uint32_t data) {
    cpu_bus->bus_store(addr, size, data);
}

// Load CSR register
uint32_t Cpu::csr_load(uint32_t csr) {
    return (uint32_t)csrs[csr];
}

// Store Csr register
void Cpu::csr_store(uint32_t csr, uint32_t value) {
    csrs[csr] = value;
}

// Print registers
void Cpu::print_reg() {
    if (print_flag) {
        std::cout << std::dec;
        std::cout << "REG[" << std::setw(2) << 0  << "] (zero)  : " << std::setw(8) << std::hex << regs[ 0] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 1  << "]  (ra)   : " << std::setw(8) << std::hex << regs[ 1] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 2  << "]  (sp)   : " << std::setw(8) << std::hex << regs[ 2] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 3  << "]  (gp)   : " << std::setw(8) << std::hex << regs[ 3] << std::dec <<"   \n";
        std::cout << "REG[" << std::setw(2) << 4  << "]  (tp)   : " << std::setw(8) << std::hex << regs[ 4] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 5  << "]  (t0)   : " << std::setw(8) << std::hex << regs[ 5] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 6  << "]  (t1)   : " << std::setw(8) << std::hex << regs[ 6] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 7  << "]  (t2)   : " << std::setw(8) << std::hex << regs[ 7] << std::dec <<"   \n";
        std::cout << "REG[" << std::setw(2) << 8  << "] (s0/fp) : " << std::setw(8) << std::hex << regs[ 8] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 9  << "]  (s1)   : " << std::setw(8) << std::hex << regs[ 9] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 10 << "]  (a0)   : " << std::setw(8) << std::hex << regs[10] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 11 << "]  (a1)   : " << std::setw(8) << std::hex << regs[11] << std::dec <<"   \n";
        std::cout << "REG[" << std::setw(2) << 12 << "]  (a2)   : " << std::setw(8) << std::hex << regs[12] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 13 << "]  (a3)   : " << std::setw(8) << std::hex << regs[13] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 14 << "]  (a4)   : " << std::setw(8) << std::hex << regs[14] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 15 << "]  (a5)   : " << std::setw(8) << std::hex << regs[15] << std::dec <<"   \n";
        std::cout << "REG[" << std::setw(2) << 16 << "]  (a6)   : " << std::setw(8) << std::hex << regs[16] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 17 << "]  (a7)   : " << std::setw(8) << std::hex << regs[17] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 18 << "]  (s2)   : " << std::setw(8) << std::hex << regs[18] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 19 << "]  (s3)   : " << std::setw(8) << std::hex << regs[19] << std::dec <<"   \n";
        std::cout << "REG[" << std::setw(2) << 20 << "]  (s4)   : " << std::setw(8) << std::hex << regs[20] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 21 << "]  (s5)   : " << std::setw(8) << std::hex << regs[21] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 22 << "]  (s6)   : " << std::setw(8) << std::hex << regs[22] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 23 << "]  (s7)   : " << std::setw(8) << std::hex << regs[23] << std::dec <<"   \n";
        std::cout << "REG[" << std::setw(2) << 24 << "]  (s8)   : " << std::setw(8) << std::hex << regs[24] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 25 << "]  (s9)   : " << std::setw(8) << std::hex << regs[25] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 26 << "]  (s10)  : " << std::setw(8) << std::hex << regs[26] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 27 << "]  (s11)  : " << std::setw(8) << std::hex << regs[27] << std::dec <<"   \n";
        std::cout << "REG[" << std::setw(2) << 28 << "]  (t3)   : " << std::setw(8) << std::hex << regs[28] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 29 << "]  (t4)   : " << std::setw(8) << std::hex << regs[29] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 30 << "]  (t5)   : " << std::setw(8) << std::hex << regs[30] << std::dec <<"   ";
        std::cout << "REG[" << std::setw(2) << 31 << "]  (t6)   : " << std::setw(8) << std::hex << regs[31] << std::dec <<"   \n";
        
        std::cout << "\n";
    }
}

// Print current R-type instruction 
void Cpu::print_R(const char* inst, uint32_t rd, uint32_t rs1, uint32_t rs2) {
    if (print_flag) {
        std::cout << "[" << inst << "] " << "REG[" << rd << "] " 
                  << "REG[" << rs1 << "] REG[" << rs2 << "]" << "\n";
    }
}

// Print current I-type instruction
void Cpu::print_I(const char* inst, uint32_t rd, uint32_t rs1, uint32_t immd) {
    if (print_flag) {
        std::cout << "[" << inst << "] " << "REG[" << rd << "] "
                  << "REG[" << rs1 << "] IMMD " << immd << " (0x" << std::hex << immd << ")\n"; 
    }
}

// Print current S-type instruction
void Cpu::print_S(const char* inst, uint32_t rs1, uint32_t rs2, uint32_t immd) {
    if (print_flag) {
        std::cout << "[" << inst << "] " << "REG[" << rs2 << "] "
                  << "REG[" << rs1 << "] IMMD " << immd << " (0x" << std::hex << immd << ")\n";
    }
}

// Print current B-type instruction
void Cpu::print_B(const char* inst, uint32_t rs1, uint32_t rs2, uint32_t immd) {
    if (print_flag) {
        std::cout << "[" << inst << "] " << "REG[" << rs1 << "] "
                  << "REG[" << rs2 << "] IMMD " << immd << " (0x" << std::hex << immd << ")\n";
    }
}

// Print current U-type instruction
void Cpu::print_U(const char* inst, uint32_t rd, uint32_t immd) {
    if (print_flag) {
        std::cout << "[" << inst << "] " << "REG[" << rd << "] "
                  << "IMMD " << immd << " (0x" << std::hex << immd << ")\n";
    }
}

// Print current J-type instruction
void Cpu::print_J(const char* inst, uint32_t rd, uint32_t immd) {
    if (print_flag) {
        std::cout << "[" << inst << "] " << "REG[" << rd << "] "
                  << "IMMD " << immd << " (0x" << std::hex << immd << ")\n";
    }
}

// Print current CSR instruction
void Cpu::print_CSR(const char* inst, uint32_t rd, uint32_t rs1, uint32_t csr) {
    if (print_flag) {
        std::cout << "[" << inst << "] " << "rd[" << rd << "] "
                  << "rs1[" << rs1 << "] csr[" << csr << "]\n";
    }
}

// Print current SYS instruction
void Cpu::print_SYS(const char* inst) {
    if (print_flag) {
        std::cout << inst << "\n";
    }
}

// R-TYPE
// - RV32I (Integer Register-Register Operation)
void Cpu::exec_ADD(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32I1) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = (uint32_t)((int32_t)regs[rs1] + (int32_t)regs[rs2]);

    print_R("ADD", rd, rs1, rs2);
}

void Cpu::exec_SUB(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32I2) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = (uint32_t)((int32_t)regs[rs1] - (int32_t)regs[rs2]);

    print_R("SUB", rd, rs1, rs2);
}

void Cpu::exec_SLL(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32I1) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = regs[rs1] << (int32_t)regs[rs2];

    print_R("SLL", rd, rs1, rs2);
}

void Cpu::exec_SLT(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32I1) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = (int32_t)((int32_t)regs[rs1] < (int32_t)regs[rs2] ? 1 : 0);

    print_R("SLT", rd, rs1, rs2);
}

void Cpu::exec_SLTU(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32I1) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = (uint32_t)regs[rs1] < (uint32_t)regs[rs2];

    print_R("SLTU", rd, rs1, rs2);

}

void Cpu::exec_XOR(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32I1) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = regs[rs1] ^ regs[rs2];

    print_R("XOR", rd, rs1, rs2);
}

void Cpu::exec_SRL(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32I1) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = regs[rs1] >> regs[rs2];

    print_R("SRL", rd, rs1, rs2);
}

void Cpu::exec_SRA(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32I2) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = (int32_t)((int32_t)regs[rs1] >> (int32_t)regs[rs2]);

    print_R("SRA", rd, rs1, rs2);
}

void Cpu::exec_OR(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32I1) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = regs[rs1] | regs[rs2];

    print_R("OR", rd, rs1, rs2);
}

void Cpu::exec_AND(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32I1) 
        illegal_inst(__LINE__);
    
    regs[rd] = regs[rs1] & regs[rs2];

    print_R("AND", rd, rs1, rs2);
}

// - RV32M Standard Extension(Integer Register-Register Operation)
void Cpu::exec_MUL(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32M) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = (int32_t)((int32_t)regs[rs1] * (int32_t)regs[rs2]);

    print_R("MUL", rd, rs1, rs2);
}

void Cpu::exec_MULH(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32M) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = (int32_t)(((int64_t)((int32_t)regs[rs1]) * (int64_t)((int32_t)regs[rs2])) >> 32);

    print_R("MULH", rd, rs1, rs2);
}

void Cpu::exec_MULHSU(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32M) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = (int32_t)(((int64_t )((int32_t )regs[rs1]) * (uint64_t)((uint32_t)regs[rs2])) >> 32);
    
    print_R("MULHSU", rd, rs1, rs2);
}

void Cpu::exec_MULHU(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32M) 
        illegal_inst(__LINE__);
    
    if (rd != 0)
        regs[rd] = (uint32_t)(((uint64_t)((uint32_t)regs[rs1]) * (uint64_t)((uint32_t)regs[rs2])) >> 32);
    
    print_R("MULHU", rd, rs1, rs2);
}

void Cpu::exec_DIV(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32M) 
        illegal_inst(__LINE__);
    
    // Error (Divide by Zero)
    if (regs[rs2] == 0) {
        regs[rd] = -1;
        return;
    }
    // Error (Signed division overflow)
    else if ((int32_t)regs[rs1] == INT32_MIN && (int32_t)regs[rs2] == -1) {
        regs[rd] = regs[rs1];
        return;
    }

    if (rd != 0) 
        regs[rd] = (int32_t)((int32_t)regs[rs1] / (int32_t)regs[rs2]);
    
    print_R("DIV", rd, rs1, rs2);
}

void Cpu::exec_DIVU(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32M) 
        illegal_inst(__LINE__);
    

    // Error (Divide by Zero)
    if (regs[rs2] == 0) {    
        regs[rd] = (uint32_t)(((uint64_t)1 << XLEN) - 1);
		return;
    }
    if (rd != 0)
        regs[rd] = regs[rs1] / regs[rs2];

    print_R("DIVU", rd, rs1, rs2);
}

void Cpu::exec_REM(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32M) 
        illegal_inst(__LINE__);
    
    // Error (Divide by Zero)
    if (regs[rs2] == 0) {
        regs[rd] = regs[rs1];
        return;
    }
    // Error (Signed Division overflow)
    else if ((int32_t)regs[rs1] == INT32_MIN && (int32_t)regs[rs2] == -1) {
        regs[rd] = 0;
        return;
    }
    regs[rd] = (int32_t)((int32_t)(regs[rs1]) % (int32_t)(regs[rs2]));

    print_R("REM", rd, rs1, rs2);
}

void Cpu::exec_REMU(uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    if (funct7 != RV32M) 
        illegal_inst(__LINE__);
    

    // Error (Divide by Zero)
    if (regs[rs2] == 0) {
        regs[rd] = regs[rs1];
        return;
    }
    regs[rd] = regs[rs1] % regs[rs2];

    print_R("REMU", rd, rs1, rs2);
}

// I-TYPE
// - JALR
void Cpu::exec_JALR(uint32_t rd, uint32_t rs1, uint32_t immd) {
    
    uint32_t addr = (regs[rs1] + immd) & (int32_t)(~1); 
    
    if (pc % 4 != 0) {
        inst_addr_misaligned(__LINE__);
    }

    next_pc = addr;

    if (rd != 0) {
        regs[rd] = pc + 4;
    }

    print_I("JALR", rd, rs1, immd);
}

// - Load Instruction
void Cpu::exec_LB(uint32_t rd, uint32_t rs1, uint32_t immd) {
    uint32_t addr = regs[rs1] + (int32_t)immd;

    if (addr % 1 != 0) 
        load_addr_misaligned(__LINE__);

    if ((addr < BASE_ADDR) && (addr > BASE_ADDR + DRAM_SIZE))
        load_access_fault(__LINE__);

    regs[rd] = (int32_t)((int8_t)cpu_load(addr, 8));

    print_I("LB", rd, rs1, immd);
}

void Cpu::exec_LH(uint32_t rd, uint32_t rs1, uint32_t immd) {
    uint32_t addr = regs[rs1] + (int32_t)immd;

    if (addr % 2 != 0) 
        load_addr_misaligned(__LINE__);

    if ((addr < BASE_ADDR) && (addr > BASE_ADDR + DRAM_SIZE))
        load_access_fault(__LINE__);

    regs[rd] = (int32_t)((int16_t)cpu_load(addr, 16));

    print_I("LH", rd, rs1, immd);
}

void Cpu::exec_LW(uint32_t rd, uint32_t rs1, uint32_t immd) {
    uint32_t addr = regs[rs1] + (int32_t)immd;

    if (addr % 4 != 0) 
        load_addr_misaligned(__LINE__);

    if ((addr < BASE_ADDR) && (addr > BASE_ADDR + DRAM_SIZE))
        load_access_fault(__LINE__);

    regs[rd] = (int32_t)((int32_t)cpu_load(addr, 32));

    print_I("LW", rd, rs1, immd);
}

void Cpu::exec_LBU(uint32_t rd, uint32_t rs1, uint32_t immd) {
    uint32_t addr = regs[rs1] + (int32_t)immd;

    if (addr % 1 != 0) 
        load_addr_misaligned(__LINE__);

    if ((addr < BASE_ADDR) && (addr > BASE_ADDR + DRAM_SIZE))
        load_access_fault(__LINE__);

    regs[rd] = cpu_load(addr, 8);

    print_I("LBU", rd, rs1, immd);
}

void Cpu::exec_LHU(uint32_t rd, uint32_t rs1, uint32_t immd) {
    uint32_t addr = regs[rs1] + (int32_t)immd;

    if (addr % 2 != 0) 
        load_addr_misaligned(__LINE__);

    if ((addr < BASE_ADDR) && (addr > BASE_ADDR + DRAM_SIZE))
        load_access_fault(__LINE__);

    regs[rd] = cpu_load(addr, 16);

    print_I("LHU", rd, rs1, immd);
}

void Cpu::exec_ADDI(uint32_t rd, uint32_t rs1, uint32_t immd) {
    if (rd != 0)
        regs[rd] = (int32_t)regs[rs1] + (int32_t)immd;

    print_I("ADDI", rd, rs1, immd);
}

void Cpu::exec_SLTI(uint32_t rd, uint32_t rs1, uint32_t immd) {
    if (rd != 0)
        regs[rd] = (int32_t)regs[rs1] < (int32_t)immd ? 1 : 0;

    print_I("SLTI", rd, rs1, immd);
}

void Cpu::exec_SLTIU(uint32_t rd, uint32_t rs1, uint32_t immd) {
    if (rd != 0)
        regs[rd] = regs[rs1] < immd ? 1 : 0;

    print_I("SLTIU", rd, rs1, immd);
}

void Cpu::exec_XORI(uint32_t rd, uint32_t rs1, uint32_t immd) {
    if (rd != 0)
        regs[rd] = regs[rs1] ^ immd;

    print_I("XORI", rd, rs1, immd);
}

void Cpu::exec_ORI(uint32_t rd, uint32_t rs1, uint32_t immd) {
    if (rd != 0)
        regs[rd] = regs[rs1] | immd;

    print_I("ORI", rd, rs1, immd);
}

void Cpu::exec_ANDI(uint32_t rd, uint32_t rs1, uint32_t immd) {
    if (rd != 0)
        regs[rd] = regs[rs1] & immd;

    print_I("ANDI", rd, rs1, immd);
}

void Cpu::exec_SLLI(uint32_t rd, uint32_t rs1, uint32_t immd, uint32_t shamt5) {
    if (rd != 0)
        regs[rd] = regs[rs1] << shamt5;

    print_I("SLLI", rd, rs1, shamt5);
}

void Cpu::exec_SRLI(uint32_t rd, uint32_t rs1, uint32_t immd, uint32_t shamt5) {
    if (rd != 0)
        regs[rd] = regs[rs1] >> shamt5;

    print_I("SRLI", rd, rs1, shamt5);
}

void Cpu::exec_SRAI(uint32_t rd, uint32_t rs1, uint32_t immd, uint32_t shamt5) {
    if (rd != 0)
        regs[rd] = (int32_t)regs[rs1] >> shamt5;

    print_I("SRAI", rd, rs1, shamt5);
}

void Cpu::exec_FENCE(uint32_t rd, uint32_t rs1, uint32_t immd) {
    print_SYS("FENCE");
}

void Cpu::exec_ECALL(uint32_t rd, uint32_t rs1, uint32_t immd) {
    print_SYS("ECALL");
}

void Cpu::exec_EBREAK(uint32_t rd, uint32_t rs1, uint32_t immd) {
    print_SYS("EBREAK");
}

void Cpu::exec_MRET(uint32_t rd, uint32_t rd1, uint32_t immd) {
    print_SYS("MRET");
}

// CSRRW instruction atomatically swaps values in the CSRs and integer registers.
// CSRRW reads the old value of the CSR, zero-extends the value to XLEN bits,
// then writes it to integer register rd.
// The initial value in rs1 is written to the CSR. 
// If rd = x0, then the instruction shall not read the CSR and 
// shall not cause any of the side effects that might occur on a CSR read.
void Cpu::exec_CSRRW(uint32_t rd, uint32_t rs1, uint32_t csr) {
    if (rd != 0)
        regs[rd] = csr_load(csr);

    csr_store(csr, regs[rs1]);

    print_CSR("CSRRW", rd, rs1, csr);
}
// Atomic Read and Set Bits in CSR
// The CSRRS instruction reads the value of the CSR, zero-extends the value to XLEN bits,
// and writes it to integer register rd.
// The initial value in integer register rs1 is treated as sa bit mask that specifies bit
// positions to be set in the CSR.
// Any bit that is high in rs1 will cause the corresponding bit to be set in the CSR,
// if that CSR bit is writeable.
// Other bits in the CSR are unaffected(through CSRs might have side effects when written).
void Cpu::exec_CSRRS(uint32_t rd, uint32_t rs1, uint32_t csr) {
    csr_store(csr, csrs[csr] | regs[rs1]);

    print_CSR("CSRRS", rd, rs1, csr);
}

// Atomic Read and Clear Bits in CSR
// The CSRRC instruction reads the value of the CSR, zero-extends the value to XLEN bits,
// and writes it to integer register rd.
// The initial value in integer register rs1 is treated as a bit mask that specifies bit
// positions to be cleared in the CSR.
// Any bit that is high in rs1 will casue the corresponding bit to cleared in the CSR,
// if that CSR bit is writable.
void Cpu::exec_CSRRC(uint32_t rd, uint32_t rs1, uint32_t csr) {
    csr_store(csr, csrs[csr] & !(regs[rs1]));

    print_CSR("CSRRS", rd, rs1, csr);
}

void Cpu::exec_CSRRWI(uint32_t rd, uint32_t rs1, uint32_t csr) {
    csr_store(csr, rs1);

    print_CSR("CSRRWI", rd, rs1, csr);
}

void Cpu::exec_CSRRSI(uint32_t rd, uint32_t rs1, uint32_t csr) {
    csr_store(csr, csrs[csr] | rs1);

    print_CSR("CSRRSI", rd, rs1, csr);
}

void Cpu::exec_CSRRCI(uint32_t rd, uint32_t rs1, uint32_t csr) {
    csr_store(csr, csrs[csr] & !(regs[rs1]));

    print_CSR("CSRRCI", rd, rs1, csr);
}

// S-TYPE
// - Store Instructions
void Cpu::exec_SB(uint32_t immd, uint32_t rs1, uint32_t rs2) {
    uint32_t addr = regs[rs1] + (int32_t)immd;
    
    if (addr % 1 != 0)
        store_addr_misaligned(__LINE__);

    if (addr < BASE_ADDR && addr > BASE_ADDR + DRAM_SIZE)
        store_access_fault(__LINE__);
    
    cpu_store(addr, 8, regs[rs2]);

    print_S("SB", rs1, rs2, immd);
}

void Cpu::exec_SH(uint32_t immd, uint32_t rs1, uint32_t rs2) {
    uint32_t addr = regs[rs1] + (int32_t)immd;
    
    if (addr % 2 != 0)
        store_addr_misaligned(__LINE__);

    if (addr < BASE_ADDR && addr > BASE_ADDR + DRAM_SIZE)
        store_access_fault(__LINE__);

    cpu_store(addr, 16, regs[rs2]);

    print_S("SH", rs1, rs2, immd);
}

void Cpu::exec_SW(uint32_t immd, uint32_t rs1, uint32_t rs2) {
    uint32_t addr = regs[rs1] + (int32_t)immd;
    
    if (addr % 4 != 0)
        store_addr_misaligned(__LINE__);

    if (addr < BASE_ADDR && addr > BASE_ADDR + DRAM_SIZE)
        store_access_fault(__LINE__);

    cpu_store(addr, 32, regs[rs2]);

    print_S("SW", rs1, rs2, immd);
}

// B-TYPE
// - Branch Instructions
void Cpu::exec_BEQ(uint32_t immd, uint32_t rs1, uint32_t rs2) {
    
    if (regs[rs1] == regs[rs2]) {
        next_pc = (int32_t)pc + (int32_t)immd;
    }

    if (next_pc % 4 != 0) 
        inst_addr_misaligned(__LINE__);

    print_B("BEG", rs1, rs2, immd);
}

void Cpu::exec_BNE(uint32_t immd, uint32_t rs1, uint32_t rs2) {
    if ((int32_t)regs[rs1] != (int32_t)regs[rs2]) {
        next_pc = (int32_t)pc + (int32_t)immd;
    }

    if (next_pc % 4 != 0) 
        inst_addr_misaligned(__LINE__);

    print_B("BNE", rs1, rs2, immd);
}

void Cpu::exec_BLT(uint32_t immd, uint32_t rs1, uint32_t rs2) {
    if ((int32_t)(regs[rs1]) < (int32_t)(regs[rs2])) {
        next_pc = (int32_t)pc + (int32_t)immd;
    }

    if (next_pc % 4 != 0) {
        inst_addr_misaligned(__LINE__);
    }

    print_B("BLT", rs1, rs2, immd);
}

void Cpu::exec_BGE(uint32_t immd, uint32_t rs1, uint32_t rs2) {
    if ((int32_t)((int32_t)regs[rs1] >= (int32_t)(regs[rs2]))) {
        next_pc = (int32_t)pc + (int32_t)immd;
    }

    if (next_pc % 4 != 0) {
        inst_addr_misaligned(__LINE__);
    }

    print_B("BGE", rs1, rs2, immd);
}

void Cpu::exec_BLTU(uint32_t immd, uint32_t rs1, uint32_t rs2) {
    if (regs[rs1] < regs[rs2]) {
        next_pc = (int32_t)pc + (int32_t)immd;
    }

    if (next_pc % 4 != 0) {
        inst_addr_misaligned(__LINE__);
    }

    print_B("BLTU", rs1, rs2, immd);
}

void Cpu::exec_BGEU(uint32_t immd, uint32_t rs1, uint32_t rs2) {
    if (regs[rs1] >= regs[rs2]) {
        next_pc = (int32_t)pc + (int32_t)immd;
    }

    if (next_pc % 4 != 0) {
        inst_addr_misaligned(__LINE__);
    }

    print_B("BGEU", rs1, rs2, immd);
}

// U-TYPE
// - LUI, AUIPC
void Cpu::exec_LUI(uint32_t rd, uint32_t immd) {
    if (rd != 0)
        regs[rd] = (int32_t)immd;

    print_U("LUI", rd, immd);
}

void Cpu::exec_AUIPC(uint32_t rd, uint32_t immd) {
    if (rd != 0)
        regs[rd] = ((int32_t)pc + (int32_t)immd);

    print_U("AUIPC", rd, immd);
}

// J-TYPE
// - JAL
void Cpu::exec_JAL(uint32_t rd, uint32_t immd) {

    if (rd != 0) 
        regs[rd] = next_pc;
    
    uint32_t addr = (int32_t)pc + (int32_t)immd;

    next_pc = addr;

    print_J("JAL", rd, immd);
}

uint32_t Cpu::get_opcode(uint32_t inst) {
    return (inst & 0x7F);           // inst[6 : 0] = opcode[6 : 0] 7 bits
}

uint32_t Cpu::get_rd(uint32_t inst) {
    return (inst >> 7) & 0x1F;      // inst[11 :  7] = rd[4 : 0] 5 bits
}

uint32_t Cpu::get_rs1(uint32_t inst) {
    return (inst >> 15) & 0x1F;     // inst[19 : 15] = rs1[4 : 0] 5 bits 
}

uint32_t Cpu::get_rs2(uint32_t inst) {
    return (inst >> 20) & 0x1F;     // inst[24 : 20] = rs2[4 : 0] 5 bits
}

uint32_t Cpu::get_funct3(uint32_t inst) {
    return (inst >> 12) & 0x7;      // inst[14 : 12] = funct3[2 : 0] 3 bits
}

uint32_t Cpu::get_funct7(uint32_t inst) {
    return (inst >> 25) & 0x7F;     // inst[31 : 25] = funct7[6 : 0] 7 bits
}

uint32_t Cpu::get_shamt5(uint32_t inst) {
    return (inst >> 20) & 0x1F;      // inst[24 : 20] = [4 : 0] 5 bits
}

uint32_t Cpu::get_immd_I(uint32_t inst) { 
    return (int32_t)inst >> 20;   // inst[31 : 20] = immd[11 : 0] 12 bits (signed extension) 
}

uint32_t Cpu::get_immd_S(uint32_t inst) {
    // inst[31 : 25] | inst[4 : 0] 12 bits (signed extension)
    return ((inst >> 7) & 0x1F)                     // inst[11 :  7] = immd[4  : 0]
        |  ((int32_t)(inst & 0xFE000000) >> 20);    // inst[31 : 25] = immd[11 : 0]
}  

uint32_t Cpu::get_immd_B(uint32_t inst) {
    // inst[31] | inst[7] | inst [30 : 25] | inst [11 : 8] 20 bits (signed extension) 
    return ((int32_t)(inst & 0x80000000) >> 19)     // inst[31] = immd[12]
        |  ((inst & 0x80) << 4)                     // inst[ 7] = immd[11]
        |  ((inst >> 20) & 0x7E0)                   // inst[30 : 25] = immd[10 : 5] 
        |  ((inst >> 7) & 0x1E);                    // inst[ 4 :  1] = immd[ 4 : 1] 
}

uint32_t Cpu::get_immd_U(uint32_t inst) {
    return (int32_t)(inst & 0xFFFFF000);            // inst[31 : 12] = immd[31 : 12] 20 bits (signd extension)
}

uint32_t Cpu::get_immd_J(uint32_t inst) {
    // inst[31] | inst[19 : 12] | inst [20] | inst[30 : 21] 20 bits (signed extension)
    return ((int32_t)(inst & 0x80000000) >> 11)     // inst[31] = immd[20]
        |  (inst & 0xFF000)                         // inst[19 : 12] = immd[19 : 12]
        |  ((inst >> 9) & 0x800)                    // inst[20] = immd[11]
        |  ((inst >> 20) & 0x7FE);                  // inst[30 : 21] = immd[10 :  1]
}

uint32_t Cpu::get_csr(uint32_t inst) {
    return ((inst & 0xFFF00000) >> 20);
}
;
bool Cpu::check_reg0() {
    return regs[0] == 0;
}

// ----------------- CPU ------------------ //
