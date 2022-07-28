#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <amogpu/amogpu.hpp>
#include "keyboard.hpp"

SDL_Window* sdl_win;
keyboard _keyboard;
bool running = true;
dynamic_batching batch;

void update_window_viewport() {
	int32_t w, h;
	SDL_GetWindowSize(sdl_win, &w, &h);

	float width = static_cast<float>(w);
	float height = static_cast<float>(h);

	// Set the viewport of window.
	glViewport(0.0f, 0.0f, width, height);
	amogpu::matrix();

	amogpu::log("Window viewport update (" + std::to_string(width) + ", " + std::to_string(height) + ")");

	// Also update the overlay stuff here, im coding in sublime so... it is hard to refactor every time.
	_keyboard.set_pos((width / 2) - (_keyboard.rect.w / 2), (height / 2) + (_keyboard.rect.h / 4));
}

void on_poll_event(SDL_Event &sdl_event) {
	switch (sdl_event.type) {
		case SDL_QUIT: {
			running = false;
			break;
		}

		case SDL_WINDOWEVENT: {
			switch (sdl_event.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED: {
					update_window_viewport();
					break;
				}
			}
		}
	}

	_keyboard.on_event(sdl_event);
}

void on_update() {

}

void on_render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(.5f, .5f, 1.0f, 1.0f);

	if (draw::refresh) {
		draw::batch.invoke();
		//draw::rectangle(50, 50, 200, 200, amogpu::vec4f(1.0f, 0.0f, 1.0f, 0.5f));
		//draw::font.render("hi sou linda", 10, 10, amogpu::vec4f(0.0f, 0.0f, 1.0f, 0.5f));

		_keyboard.on_draw_reload();
		draw::batch.revoke();
	}

	// Draw the batch.
	draw::batch.draw();

	// Draw the batch 2;
	batch.draw();
}

int main(int argv, char** argc) {
	amogpu::log("The Jogo da Forca x GPU Edition ...");
	amogpu::log("Creating window");

	SDL_Init(SDL_INIT_EVERYTHING);

	sdl_win = SDL_CreateWindow("The Jogo da Forca x GPU Edition", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(sdl_win);

	glewExperimental = true;
	glewInit();

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetSwapInterval(1); // v-sync

	glEnable(GL_DEPTH_TEST);
	amogpu::log("Window and OpenGL context created!");

	SDL_Event sdl_event;

	uint32_t cpu_fps = 60;
	uint32_t ticked_frames = 0;
	uint64_t delta_fps = 0;

	uint64_t elapsed_ticks = 0;
	uint64_t ticks_going_on = 0;
	uint64_t current_ticks = 0;
	uint64_t interval = 1000 / (uint64_t) cpu_fps;

	amogpu::log("Initinalising buffers!");
	amogpu::init();

	draw::font.load("data/fonts/impact.ttf", 30);

	_keyboard.init();
	_keyboard.calculate_scale();

	draw::refresh = true;
	update_window_viewport();

	batch.invoke();
	batch.instance(20, 20);
	batch.fill(1.0f, 1.0f, 1.0f, 1.0f); // white;

	float x = 0;
	float y = 0;

	float w = 30;
	float h = 30;

	for (uint8_t i = 0; i < 5; i++) {
		batch.vertex(x, y);
		batch.vertex(x, y + h);
		batch.vertex(x + w, y + h);
		batch.vertex(x + w, y + h);
		batch.vertex(x + w, y);
		batch.vertex(x, y);
		
		batch.coords(0.0f, 0.0f);
		batch.coords(0.0f, 0.0f);
		batch.coords(0.0f, 0.0f);
		batch.coords(0.0f, 0.0f);
		batch.coords(0.0f, 0.0f);
		batch.coords(0.0f, 0.0f);
	  
		x += w + 5;
	}

	batch.factor(x / 5); // why x / 5? we flag it as a difference.
	batch.next();
	batch.revoke();

	while (running) {
		current_ticks = SDL_GetTicks64();
		ticks_going_on = current_ticks - elapsed_ticks;

		if (ticks_going_on > interval) {
			elapsed_ticks = current_ticks;
			delta_fps += current_ticks;

			// Set the DT based on current ticks (interval ms int divided by 100... 16 int -> 0.16f);
			amogpu::clock::dt = static_cast<float>(current_ticks) / 100;

			// Flag and set the current frame rate based on CPU-ticks.
			if (delta_fps > 1000) {
				amogpu::clock::fps = ticked_frames;
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

	amogpu::log("Game shutdown complete!");
	return 1;
}