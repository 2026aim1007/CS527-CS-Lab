#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

void compile(void) {
    FILE *in = fopen("input.txt", "r");
    FILE *out = fopen("program.byte", "w");
    
    if (!in || !out) {
        printf("Error: Could not open input.txt or program.byte\n");
        exit(1);
    }

    char line[256];
    int line_num = 0;

    while (fgets(line, sizeof(line), in)) {
        line_num++;
        
        // FIX: Strip the invisible trailing newline (\n) and carriage return (\r)
        line[strcspn(line, "\r\n")] = '\0';

        // Skip empty lines (since we stripped the newline, empty lines are now just '\0')
        if (line[0] == '\0') continue;

        int op = 0, dest = 0, src1 = 0, src2 = 0;
        int matches = 0;
        int expected = 0;

        if (strncmp(line, "Read", 4) == 0) {
            op = 5;
            matches = sscanf(line, "Read x%d, %d", &dest, &src1);
            expected = 2;
        } else if (strncmp(line, "Write", 5) == 0) {
            op = 6;
            matches = sscanf(line, "Write x%d, %d", &dest, &src1);
            expected = 2;
        } else if (strchr(line, '+')) {
            op = 1;
            matches = sscanf(line, "x%d = x%d + x%d", &dest, &src1, &src2);
            expected = 3;
        } else if (strchr(line, '-')) {
            op = 2;
            matches = sscanf(line, "x%d = x%d - x%d", &dest, &src1, &src2);
            expected = 3;
        } else if (strchr(line, '*')) {
            op = 3;
            matches = sscanf(line, "x%d = x%d * x%d", &dest, &src1, &src2);
            expected = 3;
        } else if (strchr(line, '/')) {
            op = 4;
            matches = sscanf(line, "x%d = x%d / x%d", &dest, &src1, &src2);
            expected = 3;
        } else if (strchr(line, '=')) {
            op = 7;
            matches = sscanf(line, "x%d = %d", &dest, &src1);
            expected = 2;
        }

        if (op != 0) {
            // 1. SYNTAX ERROR CONTROL
            if (matches != expected) {
                // Notice the \n added at the end of the format string now
                printf("\nCOMPILATION ERROR at Line %d: '%s'\n", line_num, line);
                printf("Reason: Syntax Error. Variables must start with 'x' (e.g., x0 to x255).\n\n");
                fclose(in);
                fclose(out);
                remove("program.byte");
                exit(1);
            }

            // 2. BOUNDS ERROR CONTROL
            if (dest < 0 || dest > 255 || src1 < 0 || src1 > 255 || src2 < 0 || src2 > 255) {
                printf("\nCOMPILATION ERROR at Line %d: '%s'\n", line_num, line);
                printf("Reason: Out of Bounds.\n");
                printf("All variable indices, memory addresses, and constants must be between 0 and 255.\n\n");
                fclose(in);
                fclose(out);
                remove("program.byte");
                exit(1);
            }

            fprintf(out, "%d %d %d %d\n", op, dest, src1, src2);
        }
    }

    fprintf(out, "0 0 0 0\n");

    fclose(in);
    fclose(out);
}