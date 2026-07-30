#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

char Instruction[256];
char Data[256];

void initialize(void) {
    // Zero out memories initially
    for (int i = 0; i < 256; i++) {
        Instruction[i] = 0;
        Data[i] = 0;
    }

    // Populate Instruction Memory from program.byte
    FILE *pf = fopen("program.byte", "r");
    if (pf) {
        int op, dest, s1, s2;
        int idx = 0;
        while (fscanf(pf, "%d %d %d %d", &op, &dest, &s1, &s2) == 4 && idx < 252) {
            Instruction[idx++] = (char)op;
            Instruction[idx++] = (char)dest;
            Instruction[idx++] = (char)s1;
            Instruction[idx++] = (char)s2;
        }
        fclose(pf);
    }

    // Populate Data Memory from data.byte (if it exists)
    FILE *df = fopen("data.byte", "r");
    if (df) {
        int val, idx = 0;
        while (fscanf(df, "%d", &val) == 1 && idx < 256) {
            Data[idx++] = (char)val;
        }
        fclose(df);
    }
}

void finalize(void) {
    // Write out the modified state of data memory back to disk
    FILE *df = fopen("data.byte", "w");
    if (!df) {
        perror("Error creating data.byte during finalization");
        return;
    }
    for (int i = 0; i < 256; i++) {
        fprintf(df, "%d\n", (unsigned char)Data[i]);
    }
    fclose(df);
}