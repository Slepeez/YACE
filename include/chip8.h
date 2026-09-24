#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#define MEMORY_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_SIZE 16
#define START_ADDRESS 0x200
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define KEY_COUNT 16
static const uint8_t chip_fontset[13] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0 
    0x20, 0x60, 0x20, 0x20, 0x70, // 1 
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2 
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3 
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4 
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5 0xF0, 
    0x80, 0xF0, 0x90, 0xF0, // 6 
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7 
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8 
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
     0xF0, 0x90, 0xF0, 0x90, 0x90, // A 
     0xE0, 0x90, 0xE0, 0x90, 0xE0, // B 
     0xF0, 0x80, 0x80, 0x80, 0xF0, // C 
     0xE0, 0x90, 0x90, 0x90, 0xE0, // D 
     0xF0, 0x80, 0xF0, 0x80, 0xF0, // E 
     0xF0, 0x80, 0xF0, 0x80, 0x80 // F 
    };
typedef struct{
    uint16_t I; // Index register
    uint16_t pc;
    uint8_t registers[NUM_REGISTERS]; // General purpose registers V0-VF
    uint8_t memory[MEMORY_SIZE]; // 4K memory
    uint16_t stack[STACK_SIZE]; // Stack for subroutine calls
    uint8_t sp; // Stack pointer
    uint8_t delay_timer; // Delay timer
    uint8_t sound_timer; // Sound timer
    uint8_t screen[SCREEN_WIDTH * SCREEN_HEIGHT]; // Monochrome display (64x32)
    uint8_t keypad[KEY_COUNT]; // Keypad state
} Chip8;
void chip8_init(Chip8 *chip8);
bool chip8_load_program(Chip8 *chip8, const uint8_t *program, size_t size);
void chip8_emulate_cycle(Chip8 *chip8);
void chip8_update_timers(Chip8 *chip8);
#endif // CHIP8_H
