#include <stdio.h>
#include <string.h>
#include "compiler.h"

void compile(void) {
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("program.byte", "w");
    
    if (!in || !out) {
        printf("Error: Could not open input.txt or program.byte\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), in)) {
        if (line[0] == '\n' || line[0] == '\r') continue;

        int op = 0, dest = 0, src1 = 0, src2 = 0;

        if (strncmp(line, "Read", 4) == 0) {
            op = 5;
            sscanf(line, "Read x%d, %d", &dest, &src1);
        } else if (strncmp(line, "Write", 5) == 0) {
            op = 6;
            sscanf(line, "Write x%d, %d", &dest, &src1);
        } else if (strchr(line, '+')) {
            op = 1;
            sscanf(line, "x%d = x%d + x%d", &dest, &src1, &src2);
        } else if (strchr(line, '-')) {
            op = 2;
            sscanf(line, "x%d = x%d - x%d", &dest, &src1, &src2);
        } else if (strchr(line, '*')) {
            op = 3;
            sscanf(line, "x%d = x%d * x%d", &dest, &src1, &src2);
        } else if (strchr(line, '/')) {
            op = 4;
            sscanf(line, "x%d = x%d / x%d", &dest, &src1, &src2);
        } else if (strchr(line, '=')) {
            op = 7;
            sscanf(line, "x%d = %d", &dest, &src1);
        }

        if (op != 0) {
            fprintf(out, "%d %d %d %d\n", op, dest, src1, src2);
        }
    }

    // Append Halt operation
    fprintf(out, "0 0 0 0\n");

    fclose(in);
    fclose(out);
}