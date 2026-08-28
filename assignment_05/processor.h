#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdio.h>

#define NP 4 // Number of Processors

extern int Register[NP][256];
extern int VectorRegister[NP][32][8];
extern int PC[NP], opcode[NP], dest[NP], src1[NP], src2[NP];
extern int N[NP], Z[NP], C[NP], V[NP];
extern int end_of_simulation[NP];
extern FILE *fd_log;

void reset(int proc_id);
void fetch(int proc_id);
void decode(int proc_id);
void execute(int proc_id);
void process_instructions(int proc_id, int instruction_count);

#endif