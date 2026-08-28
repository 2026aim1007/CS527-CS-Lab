#include <stdio.h>
#include "os.h"

int main(int argc, char** argv) {
    printf("Starting Simulator OS...\n");
    printf("Type '<program.txt> <data.byte>' to load a task, or 'exit' to quit.\n");
    os_init();
        if (argc >= 3) {
        loader(argv[1], argv[2]);
    }
    scheduler();
    printf("Simulation Complete.\n");
    return 0;
}