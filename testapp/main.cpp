#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>
//#include <gpc/gui/gui.hpp>
#include <gpc/gui/root_widget.hpp>
#include <gpc/gui/gl/renderer.hpp>
#include <gpc/gui/pixel_grid/button.hpp>

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

    struct SDLPlatform {
        enum KeyCodes {
            ESCAPE = SDLK_ESCAPE
        };
    };

} // anon ns

int main(int argc, char *argv[])
{
    typedef gpc::gui::gl::Renderer<true> GLRenderer;

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

        GLRenderer renderer;

        renderer.init();

        gpc::gui::RootWidget<SDLPlatform, GLRenderer> root_widget;

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        root_widget.setBounds({0, 0}, {w, h});

        gpc::gui::pixel_grid::Button<SDLPlatform, GLRenderer> button;
        button.setBounds({100, 100}, {200, 80});
        root_widget.addChild(&button);

        root_widget.init(&renderer);

        SDL_Event event;
        bool done = false;
        while (!done) {

            renderer.enter_context();
            root_widget.repaint(&renderer, 0, 0);
            renderer.leave_context();
            SDL_GL_SwapWindow(window);

            if (!SDL_WaitEvent(&event)) throw SDLError();

            if (event.type == SDL_QUIT) done = true;
            else if (event.type == SDL_KEYDOWN) root_widget.key_down(event.key.keysym.sym);
        }
    }
    catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}