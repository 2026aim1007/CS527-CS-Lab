#include <stdio.h>
#include "memory.h"
#include "processor.h"

char Instruction[NP][256];
char Data[NP][4096];

void initialize(int proc_id, const char* prog_file, const char* data_file) {
    for (int i = 0; i < 256; i++) Instruction[proc_id][i] = 0;
    for (int i = 0; i < 4096; i++) Data[proc_id][i] = 0;
    
    FILE *p_file = fopen(prog_file, "r");
    if (p_file) {
        int op, dest, s1, s2;
        int idx = 0;
        while (fscanf(p_file, "%x %x %x %x", &op, &dest, &s1, &s2) == 4 && idx < 252) {
            Instruction[proc_id][idx++] = (char)op;
            Instruction[proc_id][idx++] = (char)dest;
            Instruction[proc_id][idx++] = (char)s1;
            Instruction[proc_id][idx++] = (char)s2;
        }
        fclose(p_file);
    }
    
    FILE *d_file = fopen(data_file, "r");
    if (d_file) {
        int v1, v2, v3, v4;
        int idx = 0;
        while (fscanf(d_file, "%x %x %x %x", &v1, &v2, &v3, &v4) == 4 && idx < 4096) {
            Data[proc_id][idx++] = (char)v1;
            Data[proc_id][idx++] = (char)v2;
            Data[proc_id][idx++] = (char)v3;
            Data[proc_id][idx++] = (char)v4;
        }
        fclose(d_file);
    }
}

void finalize(int proc_id, const char* out_data_file) {
    FILE *d_file = fopen(out_data_file, "w");
    if (d_file) {
        for (int i = 0; i < 4096; i += 4) {
            fprintf(d_file, "%02X %02X %02X %02X\n", 
                (unsigned char)Data[proc_id][i], 
                (unsigned char)Data[proc_id][i+1], 
                (unsigned char)Data[proc_id][i+2], 
                (unsigned char)Data[proc_id][i+3]);
        }
        fclose(d_file);
    }
}