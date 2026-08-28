#ifndef MEMORY_H
#define MEMORY_H

#include "processor.h"

extern char Instruction[NP][256];
extern char Data[NP][4096];

void initialize(int proc_id, const char* prog_file, const char* data_file);
void finalize(int proc_id, const char* out_data_file);

#endif