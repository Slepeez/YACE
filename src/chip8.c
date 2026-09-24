#include "chip8.h"

void chip8_init(Chip8 *chip) { 
    memset(&chip, 0, sizeof(Chip8)); 
    chip->pc = START_ADDRESS;
}
void chip8_load_program(Chip8 *chip, const uint8_t *program, size_t size) {
    if(size > MEMORY_SIZE - START_ADDRESS) {
        // Handle error: program too large to fit in memory
        return;
    }
    memcpy(&chip->memory[START_ADDRESS], program, size);
}
void chip8_emulate_cycle(Chip8 * chip){
    uint8_t opcode = chip->memory
}
