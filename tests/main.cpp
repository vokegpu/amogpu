#include <SDL2/SDL.h>
#include <amogpu/amogpu.hpp>

int32_t main(int32_t, char**) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);

    SDL_Window *p_sdl_win {SDL_CreateWindow("AmoGPU - Testing", 200, 200, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)};
    SDL_GLContext sdl_gl_context {SDL_GL_CreateContext(p_sdl_win)};

    glewExperimental = GL_TRUE;
    glewInit();

    amogpu::init();

    bool mainloop {true};
    SDL_Event sdl_event {};

    while (mainloop) {
        while (SDL_PollEvent(&sdl_event)) {
            if (sdl_event.type == SDL_QUIT) {
                mainloop = false;
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_GL_SwapWindow(p_sdl_win);
        SDL_Delay(16);
    }

    SDL_Quit();
    amogpu::quit();

    return 0;
}