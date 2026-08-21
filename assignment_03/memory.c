#include <stdio.h>
#include "memory.h"

char Instruction[256];
char Data[4096];

void initialize(void) {
    for (int i = 0; i < 256; i++) Instruction[i] = 0;
    for (int i = 0; i < 4096; i++) Data[i] = 0;
    FILE *p_file = fopen("program.byte", "r");
    if (p_file) {
        int op, dest, s1, s2;
        int idx = 0;
        while (fscanf(p_file, "%x %x %x %x", &op, &dest, &s1, &s2) == 4 && idx < 252) {
            Instruction[idx++] = (char)op;
            Instruction[idx++] = (char)dest;
            Instruction[idx++] = (char)s1;
            Instruction[idx++] = (char)s2;
        }
        fclose(p_file);
    }
    FILE *d_file = fopen("data.byte", "r");
    if (d_file) {
        int v1, v2, v3, v4;
        int idx = 0;
        // Read 4 hex bytes per line
        while (fscanf(d_file, "%x %x %x %x", &v1, &v2, &v3, &v4) == 4 && idx < 4096) {
            Data[idx++] = (char)v1;
            Data[idx++] = (char)v2;
            Data[idx++] = (char)v3;
            Data[idx++] = (char)v4;
        }
        fclose(d_file);
    } else {
        FILE *create = fopen("data.byte", "w");
        if (create) fclose(create);
    }
}

void finalize(void) {
    FILE *d_file = fopen("data.byte", "w");
    if (d_file) {
        for (int i = 0; i < 4096; i += 4) {
            fprintf(d_file, "%02X %02X %02X %02X\n", 
                (unsigned char)Data[i], 
                (unsigned char)Data[i+1], 
                (unsigned char)Data[i+2], 
                (unsigned char)Data[i+3]);
        }
        fclose(d_file);
    }
}