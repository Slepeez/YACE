#include  "display.h"
#include <stdio.h>

bool display_init(Display *display, const char *title) {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    display->window = SDL_CreateWindow(title, SCREEN_WIDTH * WINDOW_SCALE, SCREEN_HEIGHT * WINDOW_SCALE, 0);

    return true;
}