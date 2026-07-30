#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

void compile(const char *input_filename, const char *output_filename) {
    FILE *in = fopen(input_filename, "r");
    FILE *out = fopen(output_filename, "w");
    if (!in || !out) {
        perror("Compiler file error");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), in)) {
        // Skip empty lines or basic comments
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '#') continue;

        int op = 0, dest = 0, src1 = 0, src2 = 0;

        if (strncmp(line, "Read", 4) == 0) {
            op = 5;
            sscanf(line, "Read x%d, %d", &dest, &src1);
            src2 = 0;
        } 
        else if (strncmp(line, "Write", 5) == 0) {
            op = 6;
            sscanf(line, "Write x%d, %d", &dest, &src1);
            src2 = 0;
        } 
        else if (strchr(line, '+') || strchr(line, '-') || strchr(line, '*') || strchr(line, '/')) {
            char operation_char;
            sscanf(line, "x%d = x%d %c x%d", &dest, &src1, &operation_char, &src2);
            if (operation_char == '+') op = 1;
            if (operation_char == '-') op = 2;
            if (operation_char == '*') op = 3;
            if (operation_char == '/') op = 4;
        } 
        else if (strchr(line, '=')) {
            op = 7;
            sscanf(line, "x%d = %d", &dest, &src1);
            src2 = 0;
        }

        if (op != 0) {
            fprintf(out, "%d %d %d %d\n", op, dest, src1, src2);
        }
    }

    // Automatically append a HALT instruction (Opcode 0) to cleanly stop simulation
    fprintf(out, "0 0 0 0\n");

    fclose(in);
    fclose(out);
}