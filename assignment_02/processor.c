#include "processor.h"
#include "memory.h"
#include <stdint.h>

int Register[256];
int PC, opcode, dest, src1, src2;
int N, Z, C, V;
int end_of_simulation = 0;

void reset(void) {
    for (int i = 0; i < 256; i++) Register[i] = 0;
    PC = 0;
    N = 0; Z = 0; C = 0; V = 0;
    end_of_simulation = 0;
}

void fetch(void) {
    if (PC >= 252) { end_of_simulation = 1; return; }
    opcode = (unsigned char)Instruction[PC];
    dest   = (unsigned char)Instruction[PC + 1];
    src1   = (unsigned char)Instruction[PC + 2];
    src2   = (unsigned char)Instruction[PC + 3];
    PC += 4;
}

void decode(void) {}

void execute(void) {
    if (opcode == 0) { end_of_simulation = 1; return; }

    int32_t op1_val = Register[src1];
    int32_t op2_val = (opcode >= 0x09 && opcode <= 0x0F) ? src2 : Register[src2];
    if (opcode == 0x01 || opcode == 0x09) { // ADD
        int32_t res = op1_val + op2_val;
        Register[dest] = res;
        Z = (res == 0);
        N = (res < 0);
        C = ((uint32_t)res < (uint32_t)op1_val);
        V = (((op1_val ^ res) & (op2_val ^ res)) < 0);
    } 
    else if (opcode == 0x02 || opcode == 0x0A) { // SUB
        int32_t res = op1_val - op2_val;
        Register[dest] = res;
        Z = (res == 0);
        N = (res < 0);
        C = ((uint32_t)op1_val >= (uint32_t)op2_val);
        V = (((op1_val ^ op2_val) & (op1_val ^ res)) < 0);
    }
    else if (opcode == 0x03 || opcode == 0x0B) {
        Register[dest] = op1_val * op2_val;
    }
    else if (opcode == 0x04 || opcode == 0x0C) {
        if (op2_val != 0) Register[dest] = op1_val / op2_val;
        else Register[dest] = 0;
    }
    else if (opcode == 0x05 || opcode == 0x0D) {
        int32_t addr = (opcode == 0x05) ? Register[src2] : src2;
        if (addr >= 0 && addr <= 4092) {
            Register[dest] = (unsigned char)Data[addr] | 
                             ((unsigned char)Data[addr+1] << 8) | 
                             ((unsigned char)Data[addr+2] << 16) | 
                             ((unsigned char)Data[addr+3] << 24);
        }
    }
    else if (opcode == 0x06 || opcode == 0x0E) {
        int32_t addr = (opcode == 0x06) ? Register[dest] : dest;
        int32_t val = Register[src2];
        if (addr >= 0 && addr <= 4092) {
            Data[addr] = val & 0xFF;
            Data[addr+1] = (val >> 8) & 0xFF;
            Data[addr+2] = (val >> 16) & 0xFF;
            Data[addr+3] = (val >> 24) & 0xFF;
        }
    }
    else if (opcode == 0x07 || opcode == 0x0F) { 
        Register[dest] = op2_val;
    }
    else if (opcode >= 0x10 && opcode <= 0x1E) {
        int cond = opcode - 0x10;
        int met = 0;
        switch(cond) {
            case 0x0: met = (Z == 1); break;
            case 0x1: met = (Z == 0); break;
            case 0x2: met = (C == 1); break;
            case 0x3: met = (C == 0); break;
            case 0x4: met = (N == 1); break;
            case 0x5: met = (N == 0); break;
            case 0x6: met = (V == 1); break;
            case 0x7: met = (V == 0); break;
            case 0x8: met = (C == 1 && Z == 0); break;
            case 0x9: met = (C == 0 || Z == 1); break;
            case 0xA: met = (N == V); break;
            case 0xB: met = (N != V); break;
            case 0xC: met = (Z == 0 && N == V); break;
            case 0xD: met = (Z == 1 || N != V); break;
            case 0xE: met = 1; break;
        }
        if (met) {
            int8_t offset = (int8_t)src2;
            PC = (PC - 4) + (offset * 4);
        }
    }
}