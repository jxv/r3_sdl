#ifndef R3_SDL_H
#define R3_SDL_H

#include <r3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

struct r3_sdl_backend {
	SDL_Window *window;
	SDL_GLContext context;
};

bool r3_sdl_init(const char *title, v2i window_size, struct r3_ren *ren);
void r3_sdl_render(struct r3_ren *ren);
void r3_sdl_quit(struct r3_ren *ren);

#endif
