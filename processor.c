#include "processor.h"
#include "memory.h"

int Register[256];
int PC, opcode, dest, src1, src2;
int end_of_simulation = 0;

void reset(void) {
    for (int i = 0; i < 256; i++) {
        Register[i] = 0;
    }
    PC = 0;
    opcode = 0;
    dest = 0;
    src1 = 0;
    src2 = 0;
    end_of_simulation = 0;
}

void fetch(void) {
    if (PC >= 252) {
        end_of_simulation = 1;
        return;
    }
    // Read 4 bytes sequentially out of byte-addressable instruction memory
    opcode = (unsigned char)Instruction[PC];
    dest   = (unsigned char)Instruction[PC + 1];
    src1   = (unsigned char)Instruction[PC + 2];
    src2   = (unsigned char)Instruction[PC + 3];
    PC += 4; 
}

void decode(void) {
    // Intentionally left empty per professor instructions for Lab 1
}

void execute(void) {
    if (opcode == 0) {
        end_of_simulation = 1;
        return;
    }

    switch (opcode) {
        case 1: // Add
            Register[dest] = Register[src1] + Register[src2];
            break;
        case 2: // Subtract
            Register[dest] = Register[src1] - Register[src2];
            break;
        case 3: // Multiply
            Register[dest] = Register[src1] * Register[src2];
            break;
        case 4: // Divide
            if (Register[src2] != 0) {
                Register[dest] = Register[src1] / Register[src2];
            } else {
                Register[dest] = 0; // Guard against crashes
            }
            break;
        case 5: // Memory Read
            Register[dest] = (unsigned char)Data[src1];
            break;
        case 6: // Memory Write
            Data[src1] = (char)Register[dest];
            break;
        case 7: // Data Movement
            Register[dest] = src1;
            break;
        default:
            break;
    }
}