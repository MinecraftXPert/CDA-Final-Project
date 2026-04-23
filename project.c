#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
    switch (ALUControl)
    {
    case 0: // addition
        *ALUresult = A + B;
        break;
    case 1: // subtraction
        *ALUresult = A - B;
        break;
    case 2: // compare as signed
        if ((int)A < (int)B)
            *ALUresult = 1;
        else
            *ALUresult = 0;
        break;
    case 3: // compare as unsigned
        if (A < B)
            *ALUresult = 1;
        else
            *ALUresult = 0;
        break;
    case 4: // AND operation
        *ALUresult = A & B;
        break;
    case 5: // OR operation
        *ALUresult = A | B;
        break;
    case 6: // Shift B left by 16 bits
        *ALUresult = B << 16;
        break;
    case 7: // Not operation on A
        *ALUresult = ~A;
        break;
    default:
        break;
    }
    // set 0 flag
    if (*ALUresult == 0)
        *Zero = 1;
    else
        *Zero = 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
    // checks if the PC we're given is not divisble by 4 because we can't read the instruction in that case and the program will halt
    if (PC % 4 != 0)
    {
        return 1; // halt the program
    }

    // grab the instruction in it into the instruction pointer for instruction_partition to read
    *instruction = Mem[PC >> 2];

    // return 0 (meaning we can continue on)
    return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // assign everything properly using bit shifting and masking
    // first we shift to the start of where we want to grab our bits, then we mask everything by adding a bunch 1's with the width being the length of the bits and when we compare the 2 it'll give us the result we want
    // note they're written in hexadecimal to make it easier to read

    // get our op [bits 31-26] 6 bits
    *op = (instruction >> 26) & 0x3F;

    // get our r1 [bits 25-21] 5 bits
    *r1 = (instruction >> 21) & 0x1F;

    // get our r2 [bits 20-16] 5 bits
    *r2 = (instruction >> 16) & 0x1F;

    // get our r3 [bits 15-11] 5 bits
    *r3 = (instruction >> 11) & 0x1F;

    // get our funct [bits 5-0] last 6 bits
    *funct = (instruction >> 0) & 0x3F;

    // get our offset [bits 15-0] 16 bits
    *offset = (instruction >> 0) & 0xFFFF;

    // get our jsec [25-0] 26 bits
    *jsec = (instruction >> 0) & 0x3FFFFFF;
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls)
{
    // R-type
    if (op == 0)
    {
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 7; // look at funct
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 1;
    }
    // jump
    else if (op == 2)
    {
        controls->RegDst = 2; // don't care
        controls->Jump = 1;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 2; // don't care
        controls->ALUOp = 0;    // don't care
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 0;
    }
    // branch equal
    else if (op == 4)
    {
        controls->RegDst = 2; // don't care
        controls->Jump = 0;
        controls->Branch = 1;
        controls->MemRead = 0;
        controls->MemtoReg = 2; // don't care
        controls->ALUOp = 1;    // subtract for comparing
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 0;
    }
    // add immediate
    else if (op == 8)
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0; // add
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    // set less than immediate
    else if (op == 10)
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 2; // set less than
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    // set less than immediate unsigned
    else if (op == 11)
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 3; // set less than unsigned
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    // load upper immediate
    else if (op == 15)
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 6; // shift left 16
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    // load word
    else if (op == 35)
    {
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 1;
        controls->MemtoReg = 1; // data comes from memory
        controls->ALUOp = 0;    // add
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
    }
    // store word
    else if (op == 43)
    {
        controls->RegDst = 2; // don't care
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 2; // don't care
        controls->ALUOp = 0;    // add
        controls->MemWrite = 1;
        controls->ALUSrc = 1;
        controls->RegWrite = 0;
    }
    // invalid op code
    else
    {
        return 1; // halt
    }

    return 0; // all good
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
    // put into data1 whatever our first register we use is in our "array" of registers, and then do the same for register 2
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
    // extend the offset by an extended value by either 1 or 0 depending on the 16th bit (i'll delete these others later lol)
    // for example: 100 = (0110 0100). we can extend this by another 8 bits by taking the left-most bit (0) and adding 8 more zeroes in front. Then if it's for example 1 as the left-most bit, take that and add 8 1's
    // so in this case we get the left-most bit in our 16 bit offset and add 16 more 1's or 0's depending on that bit is

    // get the 16tth bit
    unsigned int bit16 = (offset >> 15) & 1;

    // if it's 0, keep it as is
    if (bit16 == 0)
    {
        *extended_value = offset;
    }
    else if (bit16 == 1)
    { // otherwise add 16 1's to the front (compare the offset with 16 1's and check with OR to add 1's to everything)
        *extended_value = (offset) | 0xFFFF0000;
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
    // check if the operator is acceptable and not R-type
    if (ALUOp < 7 && ALUOp >= 0)
    {
        // check if the extended value has to be used
        if (ALUSrc == 1)
            ALU(data1, extended_value, ALUOp, ALUresult, Zero);
        // if not, use regular data2
        else if (ALUSrc == 0)
            ALU(data1, data2, ALUOp, ALUresult, Zero);
        // ALUSrc was invalid, halt
        else
            return 1;
    }
    // check if we need R-types now
    else if (ALUOp == 7)
    {
        // switch case to handle each funct code and perform the corresponding operation
        switch (funct)
        {
        case 32:
            ALU(data1, data2, 0, ALUresult, Zero);
            break;
        case 34:
            ALU(data1, data2, 1, ALUresult, Zero);
            break;
        case 36:
            ALU(data1, data2, 4, ALUresult, Zero);
            break;
        case 37:
            ALU(data1, data2, 5, ALUresult, Zero);
            break;
        case 42:
            ALU(data1, data2, 2, ALUresult, Zero);
            break;
        case 43:
            ALU(data1, data2, 3, ALUresult, Zero);
            break;
        default:
            return 1;
        }
    }
    // invalid ALU0p, halt
    else
        return 1;

    // everything went well
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
    // check if the instruction requires accessing memory at all
    if (MemRead == 1 || MemWrite == 1)
    {
        // condition 1: address must be word-aligned
        if (ALUresult % 4 != 0)
            return 1;
        // condition 2: address can't be beyond max memory
        if (ALUresult > 0xFFFF)
            return 1;
    }
    // read from memory to memdata
    if (MemRead == 1)
        *memdata = Mem[ALUresult >> 2];
    // write data2 to memory
    if (MemWrite == 1)
        Mem[ALUresult >> 2] = data2;

    return 0; // all good
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
    /* if regdst = 0, dest in r2 ; if it = 1, dest is r3
    which data? if memtoreg = 1, write memdata
    if regwrite = 0, no data, if 1, proceed
    */

    if (RegWrite == 0)
    { // do nothing
        return;
    }
    else if (RegWrite == 1)
    {
        if (MemtoReg == 1)
        { // you write memdata
            if (RegDst == 0)
                Reg[r2] = memdata;
            else if (RegDst == 1)
                Reg[r3] = memdata;
        }
        else if (MemtoReg == 0)
        { // you write ALU result
            if (RegDst == 0)
                Reg[r2] = ALUresult;
            else if (RegDst == 1)
                Reg[r3] = ALUresult;
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{
    if (Jump == 1)
    {                                                 // we do a jump starter address (upper 4 bits)
        *PC = ((*PC + 4) & 0xF0000000) | (jsec << 2); // 0xF0000000 extract the upper 4 bits
    }
    else if (Branch == 1)
    {
        if (Zero == 1)
        {
            *PC = (*PC + 4) + (extended_value << 2);
        }
        else if (Zero == 0)
        {
            // branch isnt taken
            *PC = *PC + 4;
        }
    }

    else
    {
        // updates program count by 4 to move on to the next instruction
        *PC = *PC + 4;
    }
}