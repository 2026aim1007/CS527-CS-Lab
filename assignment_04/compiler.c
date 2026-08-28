#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "compiler.h"

struct Label {
    char name[64];
    int index;
} labels[100];
int num_labels = 0;

void compile(const char* in_filename, const char* out_filename) {
    FILE *in = fopen(in_filename, "r");
    FILE *out = fopen(out_filename, "w");
    if (!in || !out) {
        printf("Error: Could not open %s or %s\n", in_filename, out_filename);
        if (in) fclose(in);
        if (out) fclose(out);
        return;
    }
    
    num_labels = 0;
    char line[256];
    int ins_count = 0;
    
    while (fgets(line, sizeof(line), in)) {
        char *comment = strchr(line, '%');
        if (comment) *comment = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '\n' || *p == '\r') continue;
        if (p[0] == '.') {
            char label_name[64];
            sscanf(p, "%s", label_name);
            strcpy(labels[num_labels].name, label_name);
            labels[num_labels].index = ins_count;
            num_labels++;
        } else {
            ins_count++;
        }
    }

    rewind(in);
    int current_index = 0;
    int line_num = 0;
    
    while (fgets(line, sizeof(line), in)) {
        line_num++;
        char *comment = strchr(line, '%');
        if (comment) *comment = '\0';

        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '\n' || *p == '\r' || p[0] == '.') continue;
        
        if (p[0] == 'B') {
            char cond_str[10] = {0}, lab_str[64] = {0};
            if (sscanf(p, "B%9s %63s", cond_str, lab_str) == 2) {
                int cond = -1;
                if (!strcmp(cond_str, "EQ")) cond = 0x0;
                else if (!strcmp(cond_str, "NE")) cond = 0x1;
                else if (!strcmp(cond_str, "CS") || !strcmp(cond_str, "HS")) cond = 0x2;
                else if (!strcmp(cond_str, "CC") || !strcmp(cond_str, "LO")) cond = 0x3;
                else if (!strcmp(cond_str, "MI")) cond = 0x4;
                else if (!strcmp(cond_str, "PL")) cond = 0x5;
                else if (!strcmp(cond_str, "VS")) cond = 0x6;
                else if (!strcmp(cond_str, "VC")) cond = 0x7;
                else if (!strcmp(cond_str, "HI")) cond = 0x8;
                else if (!strcmp(cond_str, "LS")) cond = 0x9;
                else if (!strcmp(cond_str, "GE")) cond = 0xA;
                else if (!strcmp(cond_str, "LT")) cond = 0xB;
                else if (!strcmp(cond_str, "GT")) cond = 0xC;
                else if (!strcmp(cond_str, "LE")) cond = 0xD;
                else if (!strcmp(cond_str, "AL")) cond = 0xE;

                if (cond != -1) {
                    int target = -1;
                    for (int i = 0; i < num_labels; i++) {
                        if (strcmp(labels[i].name, lab_str) == 0) {
                            target = labels[i].index; break;
                        }
                    }
                    if (target == -1) {
                        printf("Compile Error: Label %s not found.\n", lab_str); exit(1);
                    }
                    int offset = target - current_index;
                    fprintf(out, "%02X 00 00 %02X\n", 0x10 + cond, (unsigned char)(int8_t)offset);
                    current_index++;
                    continue;
                }
            }
        }
        
        int p_reg = -1;
        if (sscanf(p, "Print x%d", &p_reg) == 1 || sscanf(p, "print x%d", &p_reg) == 1) {
            fprintf(out, "08 00 00 %02X\n", p_reg);
            current_index++;
            continue;
        }
        
        char lhs[64] = {0}, rhs[64] = {0};
        if (sscanf(p, " %[^=] = %[^\n]", lhs, rhs) == 2) {
            int d_reg = -1, d_mem_reg = -1, d_mem_const = -1, d_vec = -1;
            if (sscanf(lhs, " [x%d]", &d_mem_reg) == 1) {}
            else if (sscanf(lhs, " [%d]", &d_mem_const) == 1) {}
            else if (sscanf(lhs, " v%d", &d_vec) == 1) {}
            else if (sscanf(lhs, " x%d", &d_reg) == 1) {}

            int s1_reg=-1, s2_reg=-1, s2_const=-1, s_mem_reg=-1, s_mem_const=-1;
            int s1_vec=-1, s2_vec=-1;
            char op_char = 0;
            if (d_vec != -1) {
                if (sscanf(rhs, " v%d %c v%d", &s1_vec, &op_char, &s2_vec) == 3) {
                    int op = (op_char=='+')?0x21 : (op_char=='-')?0x22 : 0x23;
                    fprintf(out, "%02X %02X %02X %02X\n", op, d_vec, s1_vec, s2_vec);
                }
                else if (sscanf(rhs, " v%d %c x%d", &s1_vec, &op_char, &s2_reg) == 3) {
                    int op = (op_char=='+')?0x31 : (op_char=='-')?0x32 : 0x33;
                    fprintf(out, "%02X %02X %02X %02X\n", op, d_vec, s1_vec, s2_reg);
                }
                else if (sscanf(rhs, " v%d %c %d", &s1_vec, &op_char, &s2_const) == 3) {
                    int op = (op_char=='+')?0x29 : (op_char=='-')?0x2A : 0x2B;
                    fprintf(out, "%02X %02X %02X %02X\n", op, d_vec, s1_vec, s2_const);
                }
                else if (sscanf(rhs, " [x%d]", &s_mem_reg) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x25, d_vec, s_mem_reg);
                }
                else if (sscanf(rhs, " [%d]", &s_mem_const) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x2C, d_vec, s_mem_const);
                }
                else if (sscanf(rhs, " v%d", &s2_vec) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x27, d_vec, s2_vec);
                }
            }
            else if (d_reg != -1) {
                if (sscanf(rhs, " x%d %c x%d", &s1_reg, &op_char, &s2_reg) == 3) {
                    int op = (op_char=='+')?0x01 : (op_char=='-')?0x02 : (op_char=='*')?0x03 : 0x04;
                    fprintf(out, "%02X %02X %02X %02X\n", op, d_reg, s1_reg, s2_reg);
                }
                else if (sscanf(rhs, " x%d %c %d", &s1_reg, &op_char, &s2_const) == 3) {
                    int op = (op_char=='+')?0x09 : (op_char=='-')?0x0A : (op_char=='*')?0x0B : 0x0C;
                    fprintf(out, "%02X %02X %02X %02X\n", op, d_reg, s1_reg, s2_const);
                }
                else if (sscanf(rhs, " [x%d]", &s_mem_reg) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x05, d_reg, s_mem_reg);
                }
                else if (sscanf(rhs, " [%d]", &s_mem_const) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x0D, d_reg, s_mem_const);
                }
                else if (sscanf(rhs, " x%d", &s2_reg) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x07, d_reg, s2_reg); 
                }
                else if (sscanf(rhs, " %d", &s2_const) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x0F, d_reg, s2_const);
                }
            }
            else if (d_mem_reg != -1) {
                if (sscanf(rhs, " v%d", &s2_vec) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x26, d_mem_reg, s2_vec);
                } else if (sscanf(rhs, " x%d", &s2_reg) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x06, d_mem_reg, s2_reg);
                }
            }
            else if (d_mem_const != -1) {
                if (sscanf(rhs, " v%d", &s2_vec) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x2E, d_mem_const, s2_vec);
                } else if (sscanf(rhs, " x%d", &s2_reg) == 1) {
                    fprintf(out, "%02X %02X 00 %02X\n", 0x0E, d_mem_const, s2_reg);
                }
            }
            current_index++;
        } else {
            printf("Compile Error at Line %d: Unknown syntax.\n", line_num); exit(1);
        }
    }
    fprintf(out, "00 00 00 00\n");
    fclose(in); fclose(out);
}