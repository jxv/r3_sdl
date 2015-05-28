#include "r3_sdl.h"

static bool r3_sdl_init_video() {
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Couldn't initialize ren driver: %s\n", SDL_GetError());
		return false;
        }
	return true;
}

static void r3_sdl_set_gl_attributes() {
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
        SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
}

static SDL_Window *r3_sdl_create_window(const char *title, int width, int height) {
	return SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		SDL_WINDOW_OPENGL
	);
}

static bool r3_sdl_create_gl_context(SDL_Window *window, SDL_GLContext *cxt) {
	*cxt = SDL_GL_CreateContext(window);
	if (!*cxt) {
		SDL_Log("SDL_GL_CreateContext(): %s\n", SDL_GetError());
		return false;
	}
	SDL_GL_SetSwapInterval(1); // VSYNC
	int status = SDL_GL_MakeCurrent(window, *cxt);
	if (status) {
		SDL_Log("SDL_GL_MakeCurrent(): %s\n", SDL_GetError());
		return false;
	}
	return true;
}

void r3_sdl_render(struct r3_ren *ren) {
	SDL_GL_SwapWindow(((struct r3_sdl_backend*)ren->backend)->window);
}

void *r3_sdl_create_backend(const char *title, v2i window_size) {
	struct r3_sdl_backend *backend = malloc(sizeof(struct r3_sdl_backend));
	if (!r3_sdl_init_video()) {
		free(backend);
		return NULL;
	}
	r3_sdl_set_gl_attributes();
	if (!(backend->window = r3_sdl_create_window(title, window_size.x, window_size.y))) {
		free(backend);
		return NULL;
	}
	if (!r3_sdl_create_gl_context(backend->window, &backend->context)) {
		SDL_DestroyWindow(backend->window);
		free(backend);
		return NULL;
	}
	return backend;
}

void r3_sdl_quit(struct r3_ren *ren) {
	struct r3_sdl_backend *backend = ren->backend;
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_DestroyWindow(backend->window);
}

bool r3_sdl_init(const char *title, v2i window_size, struct r3_ren *ren) {
	void *backend = r3_sdl_create_backend(title, window_size);
	*ren = (struct r3_ren) {
		.window_size = window_size,
		.clear_bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
		.clear_color = _v3f(0,0,0),
		.backend = backend,
		.render = r3_sdl_render,
		.quit = r3_sdl_quit,
	};
	return true;
}

