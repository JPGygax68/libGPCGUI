#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>
#include <gpc/gui/gui.hpp>
#include <gpc/gui/gl/renderer.hpp>

namespace {

    class SDLError: public std::runtime_error {
    public:

        // TODO: context string
        SDLError(): std::runtime_error(makeErrorMessage()) {}

    private:
        auto makeErrorMessage() -> std::string {
            return std::string("SDL error: ") + SDL_GetError();
        }
    };

} // anon ns

int main(int argc, char *argv[])
{
    try {
        SDL_Init(SDL_INIT_VIDEO);

        // Create a Window
        SDL_Window *window;
        SDL_GLContext gl_ctx;
        window = SDL_CreateWindow("GPC GUI OpenGL Test Image Generator", 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
        if (!window) throw SDLError();
        gl_ctx = SDL_GL_CreateContext(window);
        if (!gl_ctx) throw SDLError();
        //glewInit(); // should be done by GLRenderer

        gpc::gui::gl::Renderer<true> renderer;

        // TODO: create a root widget

        renderer.init();
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        renderer.define_viewport(0, 0, w, h);

        SDL_Event event;
        bool done = false;
        while (!done) {
            if (!SDL_WaitEvent(&event)) throw SDLError();
            // TODO: Inject the event into the root widget (which then may or may not redraw itself)
            //renderer->enter_context();
            //renderer->leave_context();
            //SDL_GL_SwapWindow(window);
        }
    }
    catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}