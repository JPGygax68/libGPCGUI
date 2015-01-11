#include <iostream>

#include <SDL2/SDL.h>
#include <gpc/gui/gui.hpp>
#include <gpc/gui/gl/renderer.hpp>

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    // Create a Window
    SDL_Window *window;
    SDL_GLContext gl_ctx;
    window = SDL_CreateWindow("GPC GUI OpenGL Test Image Generator", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    gl_ctx = SDL_GL_CreateContext(window);
    //glewInit(); // should be done by GLRenderer

    renderer.reset(new renderer_t());
    renderer->init();
    renderer->define_viewport(0, 0, w, h);
    gen.init(renderer.get());

    // Generate the test image
    renderer->enter_context();
    auto img = gen.generate();
    renderer->leave_context();

    SDL_GL_SwapWindow(window);

    present_window(window);

    return 0;
}