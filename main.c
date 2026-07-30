#include <stdio.h>
#include "compiler.h"
#include "memory.h"
#include "processor.h"

int main(void) {
    printf("=== Starting Mini-Computer Simulation ===\n");

    // 1. Run compilation phase to generate program.byte
    compile("input.txt", "program.byte");
    printf("[COMPILER] Finished converting assembly to program.byte\n");

    // 2. Initialize System RAM
    initialize();

    // 3. Clear CPU states
    reset();

    // 4. Run the HW Fetch-Decode-Execute Loop
    printf("[PROCESSOR] Launching execution loop...\n");
    while (!end_of_simulation) {
        fetch();
        decode();
        execute();
    }
    printf("[PROCESSOR] Simulation halted safely via Opcode 0.\n");

    // 5. Commit modifications back to disk storage
    finalize();
    printf("[MEMORY] State saved successfully to data.byte\n");

    return 0;
}