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

        static void init() {

            repaint_event_id() = SDL_RegisterEvents(1);
            assert(repaint_event_id > 0);
        }

        // TODO: this might well be useless

        static void pushRepaintEvent() {

            SDL_Event event;
            SDL_zero(event);
            event.type = repaint_event_id();
            SDL_PushEvent(&event);
        }

        static Uint32 & repaint_event_id() {
            static Uint32 id = 0;
            return id;
        }
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

        SDLPlatform::init();

        GLRenderer renderer;

        renderer.init();

        typedef gpc::gui::Widget<SDLPlatform, GLRenderer> widget_t;
        gpc::gui::RootWidget<SDLPlatform, GLRenderer> root_widget;

        gpc::gui::pixel_grid::Button<SDLPlatform, GLRenderer> button(&root_widget);
        button.setCaption(L"Click me!");
        button.setBounds({100, 100}, {200, 80});
        button.addMouseEnterHandler([&](widget_t *widget, int x, int y) {
            std::cout << "Mouse entering at: " << x << ", " << y << std::endl;
            return false;
        });
        button.addMouseExitHandler([&](widget_t *widget, int x, int y) {
            std::cout << "Mouse exiting at: " << x << ", " << y << std::endl;
            return false;
        });
        button.addMouseClickHandler([&](widget_t *widget, int btn, int x, int y) {
            std::cout << "Mouse click of button " << btn << " at " << x << ", " << y << std::endl;
            button.setColor({1, 0.25f, 0});
            button.setHoverColor({ 1, 0.35f, 0.1f });
            return true;
        });

        button.setFont(root_widget.defaultFont());
        root_widget.addChild(&button);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        root_widget.defineCanvas(&renderer, w, h);

        SDL_Event event;
        bool done = false;
        while (!root_widget.terminationRequested()) {

            root_widget.updateGraphicResources();

            if (root_widget.render()) {
                SDL_GL_SwapWindow(window);
            }

            if (!SDL_WaitEvent(&event)) throw SDLError();

            if (event.type == SDL_QUIT) {
                done = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                root_widget.keyDown(event.key.keysym.sym);
            }
            else if (event.type == SDL_MOUSEMOTION) {
                root_widget.mouseMotion(event.motion.x, event.motion.y);
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                root_widget.mouseButtonDown(event.button.button, event.button.x, event.button.y);
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                root_widget.mouseButtonUp(event.button.button, event.button.x, event.button.y);
            }
            else if (event.type == SDLPlatform::repaint_event_id()) {
                std::cout << "repaint event received" << std::endl;
            }
        }

        root_widget.releaseGraphicResources();
    }
    catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}