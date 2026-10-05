#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define WINDOW_SCALE 10
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;
void display_init(Display *display, const char *title);
void display_destroy(Display *display);
void display_update(Display *display, const uint8_t *buffer);
#endif // DISPLAY_H