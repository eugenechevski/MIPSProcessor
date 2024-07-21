#include "spimcore.h"

#define MEM_END 0xFFFF

int isInstructionLegal(unsigned instr)
{
    unsigned opcode = instr >> 26;
    unsigned funct = instr & 0x3F;

    switch (opcode)
    {
    case 0x0: // R-type instructions
        switch (funct)
        {
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
/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
    // Perform the operation based on ALUControl signal
    switch (ALUControl)
    {
    case 0:
        *ALUresult = A + B; // Addition
        break;
    case 1:
        *ALUresult = A - B; // Subtraction
        break;
    case 2:
        *ALUresult = ((int)A < (int)B) ? 1 : 0; // Set on less than (signed comparison)
        break;
    case 3:
        *ALUresult = (A < B) ? 1 : 0; // Set on less than (unsigned comparison)
        break;
    case 4:
        *ALUresult = A & B; // Bitwise AND
        break;
    case 5:
        *ALUresult = A ^ B; // Bitwise XOR
        break;
    case 6:
        *ALUresult = B << 16; // Shift left logical by 16 bits
        break;
    case 7:
        *ALUresult = ~A; // Bitwise NOT
        break;
    }

    // Set Zero flag if the result is zero
    if (*ALUresult == 0)
    {
        *Zero = 1;
    }
    else
    {
        *Zero = 0;
    }
}


/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
    // Check if PC is word-aligned
    if (PC % 4 != 0)
    {
        return 1; // Halt if not word-aligned
    }

    // Check if PC is within memory bounds
    if (PC >> 2 > MEM_END)
    {
        return 1; // Halt if out of bounds
    }

    // Fetch the instruction from memory
    *instruction = Mem[PC >> 2];

    // Check if the instruction is legal
    if (!isInstructionLegal(*instruction))
    {
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
    // Initialize all control signals
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 0;
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 0;

    switch (op)
    {
    case 0x0: // R-type instructions
        controls->RegDst = 1;
        controls->RegWrite = 1;
        controls->ALUOp = 0x7;
        break;
    case 0x2: // j
        controls->Jump = 1;
        break;
    case 0x4: // beq
        controls->Branch = 1;
        controls->ALUOp = 0x1; // Subtraction for comparison
        break;
    case 0x8: // addi
    case 0xa: // slti
    case 0xb: // sltiu
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        controls->ALUOp = (op == 0x8) ? 0x0 : ((op == 0xa) ? 0x2 : 0x3);
        break;
    case 0xf: // lui
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        controls->ALUOp = 0x6; // Special ALU operation for lui
        break;
    case 0x23: // lw
        controls->ALUSrc = 1;
        controls->MemRead = 1;
        controls->RegWrite = 1;
        controls->MemtoReg = 1;
        controls->ALUOp = 0x0; // Addition for address calculation
        break;
    case 0x2b: // sw
        controls->ALUSrc = 1;
        controls->MemWrite = 1;
        controls->ALUOp = 0x0; // Addition for address calculation
        break;
    default:
        return 1; // Halt on invalid opcode
    }

    return 0; // No halt condition
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
    // Read the value from the register at r1 and store it in data1
    *data1 = Reg[r1];

    // Read the value from the register at r2 and store it in data2
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
    // If the most significant bit (bit 15) of the offset is 1, extend the sign
    if (offset & 0x00008000)
    {
        // Sign-extend the offset to 32 bits by setting the upper 16 bits to 1
        *extended_value = offset | 0xFFFF0000;
    }
    else
    {
        // If the msb is 0, value doesnt change
        *extended_value = offset;
    }
}


/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
    // If ALUSrc is asserted, we choose the sign-extended value.
    if (ALUSrc == 1)
    {
        data2 = extended_value;
    }

    if (ALUOp == 7)
    {

        switch (funct)
        {

        case 0x4:
            ALUOp = 6; // Function code 000100 set ALU to be a shift logical left by 16 bits
            break;

        case 0x20:
            ALUOp = 0; // Function code 100000 set ALU to do an addition
            break;

        case 0x21:
            ALUOp = 1; // Function code 100001 set ALU to do a subtraction
            break;

        case 0x24:
            ALUOp = 4; // Function code 100100 set ALU to do a bitwise AND
            break;

        case 0x25:
            ALUOp = 5; // Function code 100101 set ALU to do a bitwise OR
            break;

        case 0x27:
            ALUOp = 7; // Function code 100111 set ALU to do a bitwise NOT
            break;

        case 0x2A:
            ALUOp = 2; // Function code 101010 set ALU to do a Set less than
            break;

        case 0x2B:
            ALUOp = 3; // Function code 101011 set ALU to do a Set less than
            break;

        default:
            return 1; // Halt on invalid function code
        }

        // Calls ALU and perform R-type instruction function
        ALU(data1, data2, ALUOp, ALUresult, Zero);
    }
    else

        // Not an R-type instruction,performs ALUop set in Control
        ALU(data1, data2, ALUOp, ALUresult, Zero);

    // No halt condition
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{

    // Write to memory if MemWrite asserted
    if (MemWrite == 1)
    {
        // Check access to correct memory address
        if ((ALUresult % 4) != 0)
        {

            return 1; // Halt condition
        }
        else
        {
            Mem[ALUresult / 4] = data2; // misaligned memory access return Halt condition
        }
    }

    // read from memory if MemRead asserted
    if (MemRead == 1)
    {

        // Check for word alignment before reading
        if ((ALUresult % 4) != 0)
        {

            return 1; // misaligned memory access return Halt condition
        }
        else
        {
            *memdata = Mem[ALUresult / 4]; // Read data from memory
        }
    }

    // No Halt condition
    return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
    unsigned data;

    //  if whether to write data and where data comes from
    if (RegWrite == 1 && MemtoReg == 1)
    {
        data = memdata;
    }
    else if (RegWrite == 1 && MemtoReg == 0)
    {
        data = ALUresult;
    }

    unsigned reg;

    // determine address
    if (RegDst == 1)
    {
        // r type instruction
        reg = r3;
    }
    else if (RegDst == 0)
    {
        reg = r2;
    }

    Reg[reg] = data;
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{
    *PC += 4;

    // check for branching conditon
    if (Branch == 1 && Zero == 1)
    {
        *PC = *PC + (extended_value << 2);
    }

    // check for jump condition
    if (Jump == 1)
    {
        *PC = *PC = (*PC & 0xF0000000) | (jsec << 2);
    }
}
