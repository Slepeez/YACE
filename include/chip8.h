#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>
#include <stdbool.h>
#define MEMORY_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_SIZE 16
#define START_ADDRESS 0x200
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define KEY_COUNT 16
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
#endif // CHIP8_H
