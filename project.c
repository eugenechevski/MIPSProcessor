#include "spimcore.h"

#define MEM_END 0xFFFF

int isInstructionLegal(unsigned instr)
{
    unsigned opcode = instr >> 26;
    unsigned funct = instr & 0x3F;

    switch(opcode) {
        case 0x0: // R-type instructions
            switch(funct) {
                case 0x20: // add
                case 0x22: // subtract
                case 0x24: // and
                case 0x25: // or
                case 0x2a: // slt (set on less than)
                case 0x2b: // sltu (set less than unsigned)
                    return 1;
                default:
                    return 0;
            }
        case 0x8:  // addi (add immediate)
        case 0x23: // lw (load word)
        case 0x2b: // sw (store word)
        case 0xf:  // lui (load upper immediate)
        case 0x4:  // beq (branch on equal)
        case 0xa:  // slti (set less than immediate)
        case 0xb:  // sltiu (set less than immediate unsigned)
        case 0x2:  // j (jump)
            return 1;
        default:
            return 0;
    }
}

/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
    // Check if PC is word-aligned
    if (PC % 4 != 0) {
        return 1; // Halt if not word-aligned
    }

    // Check if PC is within memory bounds
    if (PC >> 2 >= MEM_END) {
        return 1; // Halt if out of bounds
    }

    // Fetch the instruction from memory
    *instruction = Mem[PC >> 2];

    // Check if the instruction is legal
    if (!isInstructionLegal(*instruction)) {
        return 1; // Halt if instruction is illegal
    }

    return 0; // No halt condition
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // Extract opcode (bits 31-26)
    *op = (instruction >> 26) & 0x3F;

    // Extract r1 (rs) (bits 25-21)
    *r1 = (instruction >> 21) & 0x1F;

    // Extract r2 (rt) (bits 20-16)
    *r2 = (instruction >> 16) & 0x1F;

    // Extract r3 (rd) (bits 15-11)
    *r3 = (instruction >> 11) & 0x1F;

    // Extract funct (bits 5-0)
    *funct = instruction & 0x3F;

    // Extract offset (bits 15-0)
    *offset = instruction & 0xFFFF;

    // Extract jsec (bits 25-0)
    *jsec = instruction & 0x3FFFFFF;
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls)
{
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{
}
