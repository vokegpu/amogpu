#include <SDL2/SDL.h>
#include <gl/glew.h>

#include "util.hpp"

SDL_Window* sdl_win;
bool running = true;

void on_poll_event(SDL_Event &sdl_event) {
	switch (sdl_event.type) {
		case SDL_QUIT: {
			running = false;
			break;
		}

		case SDL_WINDOWEVENT: {
			switch (sdl_event.window.type) {
				case SDL_WINDOWEVENT_SIZE_CHANGED: {
					// Set the viewport of window.
					glViewport(0.0f, 0.0f, static_cast<float>(sdl_event.window.data1), static_cast<float>(sdl_event.window.data2));
					break;
				}
			}
		}
	}
}

void on_update() {

}

void on_render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(.5f, .5f, 1.0f, 1.0f);
}

int main(int argv, char** argc) {
	util::log("The Jogo da Forca x GPU Edition ...");
	util::log("Creating window");

	SDL_Init(SDL_INIT_EVERYTHING);

	sdl_win = SDL_CreateWindow("The Jogo da Forca x GPU Edition", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(sdl_win);

	glewExperimental = true;
	glewInit();

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetSwapInterval(1); // v-sync

	util::log("Window and OpenGL context created!");

	SDL_Event sdl_event;

	uint32_t cpu_fps = 60;
	uint32_t ticked_frames = 0;
	uint64_t delta_fps = 0;

	uint64_t elapsed_ticks = 0;
	uint64_t ticks_going_on = 0;
	uint64_t current_ticks = 0;
	uint64_t interval = 1000 / (uint64_t) cpu_fps;

	while (running) {
		current_ticks = SDL_GetTicks64();
		ticks_going_on = current_ticks - elapsed_ticks;

		if (ticks_going_on > interval) {
			elapsed_ticks = current_ticks;
			delta_fps += current_ticks;

			// Set the DT based on current ticks (interval ms int divided by 100... 16 int -> 0.16f);
			util::clock::dt = static_cast<float>(current_ticks) / 100;

			// Flag and set the current frame rate based on CPU-ticks.
			if (delta_fps > 1000) {
				util::clock::fps = ticked_frames;
				ticked_frames = 0;
				delta_fps = 0;
			}

			// Input etc.
			while (SDL_PollEvent(&sdl_event)) {
				on_poll_event(sdl_event);
			}

			// Update and render section.
			on_update();
			on_render();

			// Count ticked frames.
			ticked_frames++;

			// Swap bufferws (front to back and back to front)
			SDL_GL_SwapWindow(sdl_win);
		}
	}

	util::log("Game shutdown complete!");
	return 1;
}