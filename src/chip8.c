#include "chip8.h"

void chip8_init(Chip8 *chip) {
  memset(chip, 0, sizeof(Chip8));
  chip->pc = START_ADDRESS;
}
bool chip8_load_program(Chip8 *chip, const uint8_t *program, size_t size) {
  if (size > MEMORY_SIZE - START_ADDRESS) {
    // Handle error: program too large to fit in memory
    return false;
  }
  memcpy(&chip->memory[START_ADDRESS], program, size);
  return true;
}
void chip8_emulate_cycle(Chip8 *chip) {
  // Fetch opcode
  uint16_t opcode =(chip->memory[chip->pc] << 8) | (chip->memory[chip->pc + 1]);
  chip->pc += 2; // Increment the program counter to point to the next instruction
  // Decode opcode
  uint8_t X = (opcode & 0x0F00) >> 8; // Extract the X register index
  uint8_t Y = (opcode & 0x00F0) >> 4; // Extract the Y register index
  uint8_t N = opcode & 0x000F;        // Extract the N value (last nibble)
  uint8_t NN = opcode & 0x00FF;       // Extract the NN value (last two bytes)
  uint16_t NNN = opcode & 0x0FFF; // Extract the NNN value (last three bytes)
  // Execute opcode
  switch (opcode & 0xF000) {
    case 0x0: // 0x0NNN: Calls RCA 1802 program at address NNN (ignored)
      switch (NN) {
      case 0xE0: // Clear the display
        memset(chip->screen, 0, sizeof(chip->screen));
        break;
    case 0xEE: // Return from subroutine
      chip->sp--;
      chip->pc = chip->stack[chip->sp];
      break;
    default:
      // Handle unknown opcode
      break;
    }
    break;
  case 0x1: // 0x1NNN: Jump to address NNN
    chip->pc = NNN;
    break;
  case 0x2:
    chip->stack[chip->sp++] = chip->pc;
    chip->pc = NNN;
    break;
  case 0x3:
    if(chip->registers[X] == NN){
      chip->pc += 2;
    }
    break;
  case 0x4:
    if(chip->registers[X] != NN){
      chip->pc += 2;
    }
    break;
  case 0x5:
    if(chip->registers[X] == chip->registers[Y]){
      chip->pc += 2;
    }
    break;
  }
}