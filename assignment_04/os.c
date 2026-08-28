#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "os.h"
#include "processor.h"
#include "memory.h"
#include "compiler.h"

typedef enum { FREE, READY, TERMINATED } ProcStatus;

ProcStatus processors[NP];
int active_tasks = 0;
int exit_flag = 0;
char shell_buffer[256];
int shell_idx = 0;

typedef struct {
    char prog[64];
    char data[64];
} Task;

Task waiting_queue[100];
int queue_head = 0, queue_tail = 0;

void os_init() {
    for (int i = 0; i < NP; i++) {
        processors[i] = FREE;
    }
    printf("$");
    fflush(stdout);
}

void loader(const char* prog_file, const char* data_file) {
    int assigned = -1;
    for (int i = 0; i < NP; i++) {
        if (processors[i] == FREE) {
            assigned = i;
            break;
        }
    }
    
    if (assigned != -1) {
        initialize(assigned, prog_file, data_file);
        reset(assigned);
        processors[assigned] = READY;
        active_tasks++;
        printf("\nOS: Task assigned to Processor %d\n", assigned);
    } else {
        strcpy(waiting_queue[queue_tail].prog, prog_file);
        strcpy(waiting_queue[queue_tail].data, data_file);
        queue_tail = (queue_tail + 1) % 100;
        printf("\nOS: All processors busy. Task placed in waiting queue.\n");
    }
}

void shell() {
    if (exit_flag) return;
    if (_kbhit()) {
        char c = _getch();
        if (c == '\r' || c == '\n') {
            printf("\n");
            shell_buffer[shell_idx] = '\0';
            if (strcmp(shell_buffer, "exit") == 0) {
                exit_flag = 1;
                printf("OS: Shell exiting. Waiting for remaining tasks to complete...\n");
            } else if (shell_idx > 0) {
                char prog[64], data[64];
                if (sscanf(shell_buffer, "%63s %63s", prog, data) == 2) {
                    char byte_prog[64];
                    snprintf(byte_prog, sizeof(byte_prog), "%s.byte", prog);
                    compile(prog, byte_prog);
                    loader(byte_prog, data);
                }
            }
            shell_idx = 0;
            if (!exit_flag) {
                printf("$");
                fflush(stdout);
            }
        } 
        else if (c == '\b' || c == 127) { 
            if (shell_idx > 0) {
                shell_idx--;
                printf("\b \b");
            }
        } 
        else {
            shell_buffer[shell_idx++] = c;
            printf("%c", c);
        }
    }
}

void scheduler() {
    while (1) {
        for (int i = 0; i < NP; i++) {
            if (processors[i] == READY) {
                process_instructions(i, 10);
                if (end_of_simulation[i]) {
                    char out_data[64];
                    snprintf(out_data, sizeof(out_data), "data_out_p%d.byte", i);
                    finalize(i, out_data);
                    processors[i] = FREE;
                    active_tasks--;
                    printf("\nOS: Processor %d finished task. Data written to %s\n", i, out_data);
                    if (queue_head != queue_tail) {
                        loader(waiting_queue[queue_head].prog, waiting_queue[queue_head].data);
                        queue_head = (queue_head + 1) % 100;
                    }
                    if (!exit_flag) {
                        printf("$"); 
                        fflush(stdout);
                    }
                }
            }
        }
        shell();
        if (exit_flag && active_tasks == 0 && queue_head == queue_tail) {
            break;
        }
    }
}