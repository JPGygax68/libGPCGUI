#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>
//#include <gpc/gui/gui.hpp>
#include <gpc/gui/root_widget.hpp>
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
    typedef gpc::gui::gl::Renderer<true> renderer_t;

    try {
        SDL_Init(SDL_INIT_VIDEO);

        // Create a Window
        SDL_Rect db;
        if (SDL_GetDisplayBounds(0, &db) < 0) throw SDLError();
        SDL_Window *window;
        SDL_GLContext gl_ctx;
        window = SDL_CreateWindow("GPC GUI OpenGL Test Image Generator", 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
            db.w, db.h, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS /*WINDOW_FULLSCREEN*/);
        if (!window) throw SDLError();
        gl_ctx = SDL_GL_CreateContext(window);
        if (!gl_ctx) throw SDLError();
        //glewInit(); // should be done by GLRenderer

        renderer_t renderer;

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        gpc::gui::RootWidget<renderer_t> root_widget(renderer, w, h);

        root_widget.init();

        SDL_Event event;
        bool done = false;
        while (!done) {

            // Repainting can be left to the root widget
            root_widget.repaint();
            // But swapping the window is specific to the platform
            SDL_GL_SwapWindow(window);

            if (!SDL_WaitEvent(&event)) throw SDLError();
        }
    }
    catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}