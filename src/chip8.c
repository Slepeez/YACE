#include "chip8.h"
#include <stdlib.h>
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
  uint16_t opcode =
      (chip->memory[chip->pc] << 8) | (chip->memory[chip->pc + 1]);
  chip->pc +=
      2; // Increment the program counter to point to the next instruction
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
  case 0x2: // 0x2NNN: Call subroutine at NNN
    chip->stack[chip->sp++] = chip->pc;
    chip->pc = NNN;
    break;
  case 0x3: // 0x3XNN: Skip next instruction if VX == NN 
    if (chip->registers[X] == NN) {
      chip->pc += 2;
    }
    break;
  case 0x4: // 0x4XNN: Skip next instruction if VX != NN
    if (chip->registers[X] != NN) {
      chip->pc += 2;
    }
    break;
  case 0x5: // 0x5XY0: Skip next instruction if VX == VY
    if (chip->registers[X] == chip->registers[Y]) {
      chip->pc += 2;
    }
    break;
  case 0x6: // 0x6XNN: Set VX to NN
    chip->registers[X] = NN;
    break;
  case 0x7: // 0x7XNN: Add NN to VX (without carry)
    chip->registers[X] += NN;
    break;
  case 0x8: // 0x8XYN: Various arithmetic and bitwise operations
   // The last nibble (N) determines the specific operation
    switch (N) {
      // Handle different arithmetic and bitwise operations based on the last nibble (N)
      case 0x0: // Set VX to the value of VY
        chip->registers[X] = chip->registers[Y];
        break;
      case 0x1: // Set VX to VX OR VY
        chip->registers[X] |= chip->registers[Y];
        break;
      case 0x2: //  Set VX to VX AND VY
        chip->registers[X] &= chip->registers[Y];
        break;
      case 0x3: // Set VX to VX XOR VY
        chip->registers[X] ^= chip->registers[Y];
        break;
      case 0x4: // Add VY to VX, set VF to 1 if there's a carry, else 0
        uint16_t result = chip->registers[X] + chip->registers[Y];
        if (result > 255) {
          chip->registers[0xF] = 1;
        } else {
          chip->registers[0xF] = 0;
        }
        chip->registers[X] = result & 0xFF;
        break;
      case 0x5: // Subtract VY from VX, set VF to 0 if there's a borrow, else 1
        if (chip->registers[X] > chip->registers[Y]) {
          chip->registers[0xF] = 1;
        } else {
          chip->registers[0xF] = 0;
        }
        chip->registers[X] -= chip->registers[Y];
        break;
      case 0x6: // Store the least significant bit of VX in VF, then shift VX right by 1
        chip->registers[0xF] = chip->registers[X] & 1;
        chip->registers[X] >>= 1;
        break;
      case 0x7: // Set VX to VY minus VX, set VF to 0 if there's a borrow, else 1
        if (chip->registers[Y] > chip->registers[X]) {
          chip->registers[0xF] = 1;
        } else {
          chip->registers[0xF] = 0;
        }
        chip->registers[X] = chip->registers[Y] - chip->registers[X];
        break;
      case 0xE: // Store the most significant bit of VX in VF, then shift VX left by 1
        chip->registers[0xF] = (chip->registers[X] & 0x80) >> 7;
        chip->registers[X] <<= 1;
        break;
      default:
        // Handle unknown opcode
        break;
    }
    case 0x9: // 0x9XY0: Skip next instruction if VX != VY
      if(chip->registers[X] != chip->registers[Y]){
        chip->pc += 2;
      }
      break;
    case 0xA: // 0xANNN: Set I to the address NNN
      chip->I = NNN;
      break;
    case 0xB: // 0xBNNN: Jump to address NNN + V0
      chip->pc = NNN + chip->registers[0];
      break;
    case 0xC: // 0xCXNN: Set VX to a random number AND NN
      chip->registers[X] = (rand() % 256) & NN;
      break;
  }
}
