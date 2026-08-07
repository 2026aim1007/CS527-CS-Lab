#include <stdio.h>
#include "compiler.h"
#include "memory.h"
#include "processor.h"

int main(void) {
    printf("Starting Simulator...\n");
    
    compile();
    initialize();
    reset();
    
    while (!end_of_simulation) {
        fetch();
        decode();
        execute();
    }
    
    finalize();
    printf("Simulation Complete. Output saved to data.byte.\n");
    
    return 0;
}