#include <stdio.h>
#include "memory.h"

char Instruction[256];
char Data[256];

void initialize(void) {
    for (int i = 0; i < 256; i++) {
        Instruction[i] = 0;
        Data[i] = 0;
    }

    FILE *p_file = fopen("program.byte", "r");
    if (p_file) {
        int op, dest, s1, s2;
        int idx = 0;
        while (fscanf(p_file, "%d %d %d %d", &op, &dest, &s1, &s2) == 4 && idx < 252) {
            Instruction[idx++] = (char)op;
            Instruction[idx++] = (char)dest;
            Instruction[idx++] = (char)s1;
            Instruction[idx++] = (char)s2;
        }
        fclose(p_file);
    }

    FILE *d_file = fopen("data.byte", "r");
    if (d_file) {
        int val, idx = 0;
        while (fscanf(d_file, "%d", &val) == 1 && idx < 256) {
            Data[idx++] = (char)val;
        }
        fclose(d_file);
    }
}

void finalize(void) {
    FILE *d_file = fopen("data.byte", "w");
    if (d_file) {
        for (int i = 0; i < 256; i++) {
            fprintf(d_file, "%d\n", (unsigned char)Data[i]);
        }
        fclose(d_file);
    }
}