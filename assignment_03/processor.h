#ifndef PROCESSOR_H
#define PROCESSOR_H

extern int Register[256];
extern int VectorRegister[32][8];
extern int PC, opcode, dest, src1, src2;
extern int N, Z, C, V;
extern int end_of_simulation;

void reset(void);
void fetch(void);
void decode(void);
void execute(void);

#endif