#include "processor.h"
#include "memory.h"
#include <stdint.h>
#include <unistd.h>

int Register[NP][256];
int VectorRegister[NP][32][8];
int PC[NP], opcode[NP], dest[NP], src1[NP], src2[NP];
int N[NP], Z[NP], C[NP], V[NP];
int end_of_simulation[NP];
FILE *fd_log = NULL;

void reset(int proc_id) {
    for (int i = 0; i < 256; i++) Register[proc_id][i] = 0;
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 8; j++) VectorRegister[proc_id][i][j] = 0;
    }
    PC[proc_id] = 0;
    N[proc_id] = 0; Z[proc_id] = 0; C[proc_id] = 0; V[proc_id] = 0;
    end_of_simulation[proc_id] = 0;
    
    if (fd_log == NULL) {
        fd_log = fopen("log.txt", "a");
    }
}

void fetch(int proc_id) {
    if (PC[proc_id] >= 252) { end_of_simulation[proc_id] = 1; return; }
    opcode[proc_id] = (unsigned char)Instruction[proc_id][PC[proc_id]];
    dest[proc_id]   = (unsigned char)Instruction[proc_id][PC[proc_id] + 1];
    src1[proc_id]   = (unsigned char)Instruction[proc_id][PC[proc_id] + 2];
    src2[proc_id]   = (unsigned char)Instruction[proc_id][PC[proc_id] + 3];
    PC[proc_id] += 4;
}

void decode(int proc_id) {}

void execute(int proc_id) {
    if (opcode[proc_id] == 0) { end_of_simulation[proc_id] = 1; return; }

    int32_t op1_val = Register[proc_id][src1[proc_id]];
    int32_t op2_val = ((opcode[proc_id] >= 0x09 && opcode[proc_id] <= 0x0F) || 
                       (opcode[proc_id] >= 0x29 && opcode[proc_id] <= 0x2B)) 
                       ? src2[proc_id] : Register[proc_id][src2[proc_id]];
    
    int op = opcode[proc_id];

    if (op == 0x01 || op == 0x09) { // ADD
        int32_t res = op1_val + op2_val;
        Register[proc_id][dest[proc_id]] = res;
        Z[proc_id] = (res == 0); N[proc_id] = (res < 0);
        C[proc_id] = ((uint32_t)res < (uint32_t)op1_val);
        V[proc_id] = (((op1_val ^ res) & (op2_val ^ res)) < 0);
    } 
    else if (op == 0x02 || op == 0x0A) { // SUB
        int32_t res = op1_val - op2_val;
        Register[proc_id][dest[proc_id]] = res;
        Z[proc_id] = (res == 0); N[proc_id] = (res < 0);
        C[proc_id] = ((uint32_t)op1_val >= (uint32_t)op2_val);
        V[proc_id] = (((op1_val ^ op2_val) & (op1_val ^ res)) < 0);
    }
    else if (op == 0x03 || op == 0x0B) { // MUL
        Register[proc_id][dest[proc_id]] = op1_val * op2_val;
    }
    else if (op == 0x04 || op == 0x0C) { // DIV
        if (op2_val != 0) Register[proc_id][dest[proc_id]] = op1_val / op2_val;
        else Register[proc_id][dest[proc_id]] = 0;
    }
    else if (op == 0x05 || op == 0x0D) { // Scalar Read
        int32_t addr = (op == 0x05) ? Register[proc_id][src2[proc_id]] : src2[proc_id];
        if (addr >= 0 && addr <= 4092) {
            Register[proc_id][dest[proc_id]] = (unsigned char)Data[proc_id][addr] | 
                             ((unsigned char)Data[proc_id][addr+1] << 8) | 
                             ((unsigned char)Data[proc_id][addr+2] << 16) | 
                             ((unsigned char)Data[proc_id][addr+3] << 24);
        }
    }
    else if (op == 0x06 || op == 0x0E) { // Scalar Write
        int32_t addr = (op == 0x06) ? Register[proc_id][dest[proc_id]] : dest[proc_id];
        int32_t val = Register[proc_id][src2[proc_id]];
        if (addr >= 0 && addr <= 4092) {
            Data[proc_id][addr]   = val & 0xFF;
            Data[proc_id][addr+1] = (val >> 8) & 0xFF;
            Data[proc_id][addr+2] = (val >> 16) & 0xFF;
            Data[proc_id][addr+3] = (val >> 24) & 0xFF;
        }
    }
    else if (op == 0x07 || op == 0x0F) { // Move
        Register[proc_id][dest[proc_id]] = op2_val;
    }
    else if (op == 0x08) { // Print 
        if (fd_log) {
            fprintf(fd_log, "Process id: %d x%d : %X\n", proc_id, src2[proc_id], Register[proc_id][src2[proc_id]]);
            fflush(fd_log);
        }
    }
    else if (op >= 0x10 && op <= 0x1E) { // Branch
        int cond = op - 0x10;
        int met = 0;
        switch(cond) {
            case 0x0: met = (Z[proc_id] == 1); break;
            case 0x1: met = (Z[proc_id] == 0); break;
            case 0x2: met = (C[proc_id] == 1); break;
            case 0x3: met = (C[proc_id] == 0); break;
            case 0x4: met = (N[proc_id] == 1); break;
            case 0x5: met = (N[proc_id] == 0); break;
            case 0x6: met = (V[proc_id] == 1); break;
            case 0x7: met = (V[proc_id] == 0); break;
            case 0x8: met = (C[proc_id] == 1 && Z[proc_id] == 0); break;
            case 0x9: met = (C[proc_id] == 0 || Z[proc_id] == 1); break;
            case 0xA: met = (N[proc_id] == V[proc_id]); break;
            case 0xB: met = (N[proc_id] != V[proc_id]); break;
            case 0xC: met = (Z[proc_id] == 0 && N[proc_id] == V[proc_id]); break;
            case 0xD: met = (Z[proc_id] == 1 || N[proc_id] != V[proc_id]); break;
            case 0xE: met = 1; break;
        }
        if (met) {
            int8_t offset = (int8_t)src2[proc_id];
            PC[proc_id] = (PC[proc_id] - 4) + (offset * 4);
        }
    }
    else if (op >= 0x21 && op <= 0x33) { // Vector Arithmetic
        int32_t result[8];
        for(int i = 0; i < 8; i++) {
            int32_t v1 = VectorRegister[proc_id][src1[proc_id]][i];
            int32_t v2;
            if (op >= 0x21 && op <= 0x23) v2 = VectorRegister[proc_id][src2[proc_id]][i]; 
            else if (op >= 0x31 && op <= 0x33) v2 = Register[proc_id][src2[proc_id]];     
            else v2 = src2[proc_id];                                                      
            
            int math_type = op % 0x10;
            if (math_type == 1 || math_type == 9) result[i] = v1 + v2;
            else if (math_type == 2 || math_type == 0xA) result[i] = v1 - v2;
            else result[i] = v1 * v2;
        }
        for(int i = 0; i < 8; i++) {
            VectorRegister[proc_id][dest[proc_id]][i] = result[i];
        }
    }
    else if (op == 0x25 || op == 0x2C) { // Vector Memory Read
        int32_t base_addr = (op == 0x25) ? Register[proc_id][src2[proc_id]] : src2[proc_id];
        for (int i = 0; i < 8; i++) {
            int32_t addr = base_addr + (i * 4);
            if (addr >= 0 && addr <= 4088) {
                VectorRegister[proc_id][dest[proc_id]][i] = (unsigned char)Data[proc_id][addr] | 
                                          ((unsigned char)Data[proc_id][addr+1] << 8) | 
                                          ((unsigned char)Data[proc_id][addr+2] << 16) | 
                                          ((unsigned char)Data[proc_id][addr+3] << 24);
            }
        }
        if (op == 0x25) Register[proc_id][src2[proc_id]] += 32; 
    }
    else if (op == 0x26 || op == 0x2E) { // Vector Memory Write
        int32_t base_addr = (op == 0x26) ? Register[proc_id][dest[proc_id]] : dest[proc_id];
        for (int i = 0; i < 8; i++) {
            int32_t addr = base_addr + (i * 4);
            int32_t val = VectorRegister[proc_id][src2[proc_id]][i];
            if (addr >= 0 && addr <= 4088) {
                Data[proc_id][addr]   = val & 0xFF;
                Data[proc_id][addr+1] = (val >> 8) & 0xFF;
                Data[proc_id][addr+2] = (val >> 16) & 0xFF;
                Data[proc_id][addr+3] = (val >> 24) & 0xFF;
            }
        }
        if (op == 0x26) Register[proc_id][dest[proc_id]] += 32; 
    }
    else if (op == 0x27) { // Vector Move
        for(int i = 0; i < 8; i++) VectorRegister[proc_id][dest[proc_id]][i] = VectorRegister[proc_id][src2[proc_id]][i];
    }
}

void process_instructions(int proc_id, int instruction_count) {
    for(int i = 0; i < instruction_count; i++) {
        if (end_of_simulation[proc_id]) break;
        fetch(proc_id);
        decode(proc_id);
        execute(proc_id);
    }
    usleep(10000);
}