#include "spimcore.h"

/*
    NOTES:

    TODO: REMEMBER TO DELETE ANY PRINT STATEMENTS WITH [DEBUG] IN THEM AT THE END (or something at the beginning of it)

    * Remember if there's any sort of debugging you want to do remember to put [DEBUG] or something like that so we know to delete them before submitting

    * I made sure to add lots of comments to help you understand the code a bit better lol
*/

/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{
}

/* instruction fetch */
/* 10 Points */
// DONE
//  NOTE: There's a lot of print statements to make sense of everything but it'll get deleted once I'm done and it'll only be like 3 or 4 lines of code lol
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{

    // testing stuff
    printf("[DEBUG IF] PC: %04x\n", PC);
    printf("[DEBUG IF] Memory: %08x\n", Mem[PC >> 2]);

    // checks if the PC we're given is not divisble by 4 because we can't read the instruction in that case and the program will halt
    if (PC % 4 != 0)
    {
        return 1; // halt the program
    }

    // grab the instruction in it into the instruction pointer for instruction_partition to read
    *instruction = Mem[PC >> 2];

    // finally worked lol
    printf("[DEBUG IF] Instruction in hexadecimal: %08x\n", *instruction);

    // return 0 (meaning we can continue on)
    return 0;
}

/* instruction partition */
/* 10 Points */
// DONE
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // assign everything properly using bit shifting and masking
    // first we shift to the start of where we want to grab our bits, then we mask everything by adding a bunch 1's with the width being the length of the bits and when we compare the 2 it'll give us the result we want
    // note they're written in hexadecimal to make it easier to read

    // get our op [bits 31-26] 6 bits
    *op = (instruction >> 26) & 0x3F;
    printf("[DEBUG IP] OP: %u\n", *op);

    // get our r1 [bits 25-21] 5 bits
    *r1 = (instruction >> 21) & 0x1F;
    printf("[DEBUG IP] r1: %u\n", *r1);

    // get our r2 [bits 20-16] 5 bits
    *r2 = (instruction >> 16) & 0x1F;
    printf("[DEBUG IP] r2: %u\n", *r2);

    // get our r3 [bits 15-11] 5 bits
    *r3 = (instruction >> 11) & 0x1F;
    printf("[DEBUG IP] r3: %u\n", *r3);

    // get our funct [bits 5-0] last 6 bits
    *funct = (instruction >> 0) & 0x3F;
    printf("[DEBUG IP] funct: %u\n", *funct);

    // get our offset [bits 15-0] 16 bits
    *offset = (instruction >> 0) & 0xFFFF;
    printf("[DEBUG IP] Offset: %u\n", *offset);

    // get our jsec [25-0] 26 bits
    *jsec = (instruction >> 0) & 0x3FFFFFF;
    printf("[DEBUG IP] jsec: %u\n", *jsec);
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls)
{
    // decode the op to tell the control what to do

    // if the op is addi
    if (op == 8)
    {
        // we're not writing or reading anything so don't enable them
        controls->MemRead = 0;
        controls->MemWrite = 0;

        // enable register write since we are writing to the register
        controls->RegWrite = 1;

        controls->RegDst = 0;
        controls->MemtoReg = 0;
        controls->ALUSrc = 1;

        // tell the alu to do addition since we're adding
        controls->ALUOp = 0;
    }

    // if the op is lw
    else if (op == 35)
    {

        // you read from memory to load into register
        controls->MemRead = 1;
        controls->MemWrite = 0;
        controls->RegWrite = 1;
    }

    // if the op is sw
    else if (op == 43)
    {
        // you write from register and put it in memory (so you don't have to read from memory or write any registers)
        controls->MemRead = 0;
        controls->MemWrite = 1;
        controls->RegWrite = 0;
    }

    // if the op is lui (load upper immediate)
    else if (op == 15)
    {
    }

    // if the op is beq (branch equal)
    else if (op == 4)
    {
    }

    // if the op is slti (set less than immediate)
    else if (op == 10)
    {
    }

    // if the op is sltiu (set less than immediate unsigned)
    else if (op == 11)
    {
    }

    // if the op is j (jump)
    else if (op == 2)
    {
    }

    // the rest of the r-type instructions
    else if (op == 0)
    {
        controls->ALUOp = 7;
    }

    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2)
{
    // i think this is how it's supposed to be done but idk lol I don't think it's too complicated

    // put into data1 whatever our first register we use is in our "array" of registers, and then do the same for register 2
    *data1 = Reg[r1];
    *data2 = Reg[r2];
    printf("[DEBUG RR] Data 1: %u\n", *data1);
    printf("[DEBUG RR] Data 2: %u\n", *data2);
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
    printf("[DEBUG SE] Extended Offset: %08x\n", *extended_value);
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
    return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
    // Write the data (ALUresult or memdata) to a register (Reg) addressed by r2 or r3
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
    // updates program count by 4 to move on to the next instruction
    *PC = *PC + 4;
}