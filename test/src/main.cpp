#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <amogpu/amogpu.hpp>
#include "keyboard.hpp"

SDL_Window* sdl_win;
keyboard keyklass;
bool running = true;
dynamic_batching batch;

void update_window_viewport() {
	int32_t w, h;
	SDL_GetWindowSize(sdl_win, &w, &h);

	float width = static_cast<float>(w);
	float height = static_cast<float>(h);

	// Set the viewport of window.
	glViewport(0.0f, 0.0f, width, height);
	amogpu::log("Window viewport update (" + std::to_string(width) + ", " + std::to_string(height) + ")");

	// Also update the overlay stuff here, im coding in sublime so... it is hard to refactor every time.
	keyklass.set_pos((width / 2) - (keyklass.rect.w / 2), (height / 2) + (keyklass.rect.h / 4));
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

	keyklass.on_event(sdl_event);
}

void on_update() {

}

void on_render() {
	amogpu::matrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(.5f, .5f, 1.0f, 1.0f);

	if (draw::refresh) {
		draw::batch.invoke();
		draw::font.render("hi the fps: " + std::to_string(amogpu::clock::fps), 10, 10, amogpu::vec4f(0.0f, 0.0f, 1.0f, 0.5f));
		draw::batch.revoke();
	}

	// Draw the batch.
	draw::batch.draw();
}

int main(int argv, char** argc) {
	amogpu::log("The Jogo da Forca x GPU Edition ...");
	amogpu::log("Creating window");

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	sdl_win = SDL_CreateWindow("The Jogo da Forca x GPU Edition", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(sdl_win);

	glewExperimental = true;
	glewInit();

	glDisable(GL_DEPTH_TEST);
	amogpu::log("Window and OpenGL context created!");

	SDL_Event sdl_event;

	uint32_t cpu_fps = 60;
	uint32_t ticked_frames = 0;
	uint64_t delta_fps = 0;

	uint64_t elapsed_ticks = 0;
	uint64_t ticks_going_on = 0;
	uint64_t current_ticks = 0;
	uint64_t interval = 1000 / (uint64_t) cpu_fps;

	bool no_vsync = false;
	SDL_GL_SetSwapInterval(no_vsync); // v-sync

	amogpu::log("Initinalising buffers!");
	amogpu::init();

	draw::font.load("data/fonts/impact.ttf", 30);

	keyklass.init();
	keyklass.calculate_scale();

	draw::refresh = true;
	update_window_viewport();

	shape_builder shape;

	batch.invoke();
	batch.instance(200, 200);
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
	x = 0;
	y = 0;

	w = 1.0f;
	h = 1.0f;

	batch.next();
	batch.instance(500, 50);
	batch.bind(draw::font.texture_bitmap);
	batch.fill(1.0f, 1.0f, 1.0f, 1.0f);
	batch.modal(300, 300);
	batch.vertex(x, y);
	batch.vertex(x, y + h);
	batch.vertex(x + w, y + h);
	batch.vertex(x + w, y + h);
	batch.vertex(x + w, y);
	batch.vertex(x, y);

	x = 0.922495f;
	w = 0.008192f;
	y = 0.000000f;
	h = 0.678571f;

	batch.coords(x, y);
	batch.coords(x, y + h);
	batch.coords(x + w, y + h);
	batch.coords(x + w, y + h);
	batch.coords(x + w, y);
	batch.coords(x, y);
	batch.next();

	batch.revoke();

	while (running) {
		current_ticks = SDL_GetTicks64();
		ticks_going_on = current_ticks - elapsed_ticks;

		if (ticks_going_on > interval || no_vsync) {
			elapsed_ticks = current_ticks;
			delta_fps += ticks_going_on;

			// Set the DT based on current ticks (interval ms int divided by 100... 16 int -> 0.16f);
			amogpu::clock::dt = static_cast<float>(current_ticks) / 100;

			// Flag and set the current frame rate based on CPU-ticks.
			if (delta_fps > 1000) {
				amogpu::clock::fps = ticked_frames;
				ticked_frames = 0;
				delta_fps = 0;
				draw::refresh = true;
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

//uint32_t screen_width, screen_height;
//
//int main(int argv, char** argc) {
//	// Iniciamos o SDL.
//	SDL_Init(SDL_INIT_EVERYTHING);
//
//	// Definimos o contexto apenas para GL moderno (3+).
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
//	SDL_GL_SetSwapInterval(1); // Ligamos o VSYNC pra não usar 100% da GPU.
//
//	// Criamos a janela do SDL2.
//	sdl_win = SDL_CreateWindow("hi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_OPENGL);
//
//	// Agora criamos o contexto OpenGL.
//	SDL_GLContext sdl_gl_context = SDL_GLContext(sdl_win);
//
//	// Habilitamos as extensões do opengl.
//	glewExperimental = GL_TRUE;
//	glewInit();
//
//	uint64_t fps = 0;
//	uint64_t ticked_frames = 0;
//	uint64_t delta = 0;
//
//	uint64_t elapsed_ticks = 0;
//	uint64_t ticks_going_on = 0;
//	uint64_t current_ticks = 0;
//	uint16_t ticks_interval = 1000 / 60; // a um intervalo de 16ms cada tick.
//
//	float dt = 0.0f;
//	SDL_Event sdl_event;
//
//	/*
//	 * Mainloop.
//	 */
//	while (running) {
//		current_ticks = SDL_GetTicks64();
//		ticks_going_on = current_ticks - current_ticks;
//
//		if (ticks_going_on > ticks_interval) {
//			elapsed_ticks = current_ticks; // resetamos e esperamos 16ms agora dps desse segmento.
//			dt = static_cast<float>(current_ticks) / 100.0f;
//			delta += current_ticks;
//			
//			// Contamos o fps (se for maior que 1s então resetamos).
//			if (delta > 1000) {
//				fps = ticked_frames;
//				delta = 0;
//			}
//
//			while (SDL_PollEvent(&sdl_event)) {
//				switch (sdl_event.type) {
//					running = false;
//					break;
//				}
//			}
//
//			// ...
//			// Segmento de atualização.
//
//			// ...
//			// Segmento de renderização.
//			glViewport(0.0f, 0.0f, static_cast<float>(screen_width), static_cast<float>(screen_height));
//			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//			glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
//
//			// Prontinho podemos renderizar.
//			// Bem aqui.
//
//			// Contams os ticks.
//			ticked_frames++;
//
//			// Swap buffers.
//			SDL_GL_SwapWindow(sdl_win);
//		}
//	}
//}
