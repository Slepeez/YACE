#include "chip8.h"
#include <stdlib.h>
#include <stdio.h>
void chip8_init(Chip8 *chip) {
  // Initialize the Chip8 system
  memset(chip, 0, sizeof(Chip8));
  chip->pc = START_ADDRESS;
  memcpy(chip->memory[0x050], fontset, sizeof(fontset)); // Load fontset into memory
}
bool chip8_load_program(Chip8 *chip, const uint8_t *program, size_t size) {
  FILE *file = fopen(program, "rb");
  if (!file) {
    fprintf(stderr, "Failed to open program file: %s\n", program);
    return false;
  }
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  // Check if the program size exceeds available memory
  if (file_size > MEMORY_SIZE - START_ADDRESS) {
    fprintf(stderr, "Program size exceeds available memory.\n");
    fclose(file);
    return false;
  }
  // Load the program into memory starting at the start address
  size_t bytes_read = fread(chip->memory + START_ADDRESS, 1, file_size, file);
  if (bytes_read != file_size) {
    fprintf(stderr, "Failed to read the entire program file.\n");
    fclose(file);
    return false;
  }
  fclose(file);
  return true;
}
void chip8_emulate_cycle(Chip8 *chip) {
  // Fetch opcode
  uint16_t opcode = (chip->memory[chip->pc] << 8) | (chip->memory[chip->pc + 1]);
  chip->pc += 2; // Increment the program counter to point to the next instruction
  // Decode opcode
  uint8_t X = (opcode & 0x0F00) >> 8; // Extract the X register index
  uint8_t Y = (opcode & 0x00F0) >> 4; // Extract the Y register index
  uint8_t N = opcode & 0x000F;        // Extract the N value (last nibble)
  uint8_t NN = opcode & 0x00FF;       // Extract the NN value (last two bytes)
  uint16_t NNN = opcode & 0x0FFF;
  printf("Opcode: %04X, PC: %04X, I: %04X\n", opcode, chip->pc, chip->I);
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
        if (chip->registers[X] >= chip->registers[Y]) {
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
        if (chip->registers[Y] >= chip->registers[X]) {
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
    break;
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
    case 0xD:{ // 0xDXYN: Draw a sprite at coordinate (VX, VY) with width 8 pixels and height N pixels
      uint8_t x_pos = chip->registers[X] % SCREEN_WIDTH;
      uint8_t y_pos = chip->registers[Y] % SCREEN_HEIGHT;

      chip->registers[0xF] = 0;

      for (int row = 0; row < N; row++) {
        uint8_t sprite_byte = chip->memory[chip->I + row];

        for (int col = 0; col < 8; col++) {
           uint8_t sprite_pixel = sprite_byte & (0x80 >> col);

          if (sprite_pixel) {
              if (x_pos + col < SCREEN_WIDTH && y_pos + row < SCREEN_HEIGHT) {
                  int idx = (y_pos + row) * SCREEN_WIDTH + (x_pos + col);

                  if (chip->screen[idx] == 1) {
                      chip->registers[0xF] = 1;
                  }

                chip->screen[idx] ^= 1;
              }
          }
        }
      } 
      break;
    } 
    case 0xE: // 0xEX9E and 0xEXA1: Skip next instruction based on key press
      switch(NN) {
        case 0x9E:
          if(chip->keypad[chip->registers[X]]){
            chip->pc += 2; // Skip next instruction if key in VX is pressed
          }
          break;
        case 0xA1:
          if(!chip->keypad[chip->registers[X]]){
            chip->pc += 2; // Skip next instruction if key in VX is not pressed
          }
          break;
        default:
          // Handle unknown opcode
          break;
      }
    case 0xF: // 0xFXNN: Various operations based on the last two bytes (NN)
      switch(NN){
        case 0x07:
          chip->registers[X] = chip->delay_timer; // Set VX to the value of the delay timer
          break;
        case 0x0A: 
          // Wait for a key press and store the result in VX
          int key_pressed = -1;
          for(int i = 0; i < KEY_COUNT; i++){
            if(chip->keypad[i]){
              key_pressed = i;
              chip->registers[X] = i; // Store the key index in VX
              break;
            }
          }
          if(key_pressed == -1){
            chip->pc -= 2; // Repeat this instruction until a key is pressed
          }
          break;
        case 0x15:
          chip->delay_timer = chip->registers[X]; // Set the delay timer to VX
          break;
        case 0x18:
          chip->sound_timer = chip->registers[X]; // Set the sound timer to VX
          break;
        case 0x1E:
          chip->I += chip->registers[X]; // Add VX to I
          break;
        case 0x29: // Set I to the location of the sprite for the character in VX (fontset)
          chip->I = 0x050 + (chip->registers[X] * 5); // Each character is 5 bytes long
          break;
        case 0x33: // Store the binary-coded decimal representation of VX at I, I+1, and I+2
          chip->memory[chip->I] = (chip->registers[X] / 100) % 10; // Hundreds digit
          chip->memory[chip->I + 1] = (chip->registers[X] / 10) % 10; // Tens digit
          chip->memory[chip->I + 2] = chip->registers[X] % 10; // Ones digit
          break;
        case 0x55: // Store registers V0 through VX in memory starting at address I
          for(int i = 0; i <= X; i++){
            chip->memory[chip->I + i] = chip->registers[i];
          }
          break;
        case 0x65: // Read registers V0 through VX from memory starting at address
          for(int i = 0; i <= X; i++){
            chip->registers[i] = chip->memory[chip->I + i];
          }
          break;
        default:
          // Handle unknown opcode
          break;
      }
      break;
  }
}
void chip8_update_timers(Chip8 *chip) {
  if (chip->delay_timer > 0) {
    chip->delay_timer--;
  }
  if (chip->sound_timer > 0) {
    chip->sound_timer--;
  }
}