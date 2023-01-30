#ifndef __OPCODE_H__
#define __OPCODE_H__

// R_TYPE
// Integer Register-Register Operations
#define REG_REG 0b0110011
    #define RV32I1  0b0000000   // funct7
        #define ADD     0b000
        #define SLL     0b001
        #define SLT     0b010
        #define SLTU    0b011
        #define XOR     0b100
        #define SRL     0b101
        #define OR      0b110
        #define AND     0b111     
    #define RV32I2  0b0100000   // funct7
        #define SUB     0b000 
        #define SRA     0b101
    
    // RV32M Standard Extension (Integer Register-Register Operation)
    #define RV32M    0b0000001   // funct7
        #define MUL     0b000
        #define MULH    0b001
        #define MULHSU  0b010
        #define MULHU   0b011
        #define DIV     0b100
        #define DIVU    0b101
        #define REM     0b110
        #define REMU    0b111

// I_TYPE 
#define JALR    0b1100111

// Load instructions
#define LOAD    0b0000011
    #define LB      0b000
    #define LH      0b001
    #define LW      0b010
    #define LBU     0b100
    #define LHU     0b101

// Integer Register-Immediate Operations
#define REG_IMM 0b0010011
    #define ADDI    0b000
    #define SLTI    0b010
    #define SLTIU   0b011
    #define XORI    0b100
    #define ORI     0b110
    #define ANDI    0b111
    #define SLLI    0b001
    #define SRI     0b101
        #define SRLI    0b0000000
        #define SRAI    0b0100000

// Memory Ordering Instructions
// - FENCE
#define FENCE   0b0001111

// Environment Call and Breakpoints
// + Zicsr Standard Extension
#define SYSTEM  0b1110011
    // RV32I Base Instructions Set (ECALL, EBREAK)
    #define CB      0b000
        #define ECALL   0b000000000000
        #define EBREAK  0b000000000001
        #define MRET    0b001100000010
    // RV32 Zicsr Standard Extension
    #define CSRRW   0b001
    #define CSRRS   0b010
    #define CSRRC   0b011
    #define CSRRWI  0b101
    #define CSRRSI  0b110
    #define CSRRCI  0b111   


// S-TYPE 
// Store instructions
#define STORE   0b0100011
    #define SB      0b000
    #define SH      0b001
    #define SW      0b010

// B-TYPE
#define BRANCH  0b1100011
    #define BEG     0b000
    #define BNE     0b001
    #define BLT     0b100
    #define BGE     0b101
    #define BLTU    0b110
    #define BGEU    0b111
    
// U-TYPE 
#define LUI     0b0110111
#define AUIPC   0b0010111

// J-TYPE 
#define JAL     0b1101111

#endif

