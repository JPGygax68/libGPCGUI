#pragma once

#include "widget.hpp"

namespace gpc {

    namespace gui {

        template <class Renderer, class Platform>
        class RootWidget: public Widget<Renderer> {

        public:

            /** TODO: We create the root widget with an initial size; this is based on the assumption
                that the display has already been created at this point. It might be better to
                leave this to an attach() method rather than to involve the constructor - I haven't 
                really decided yet.
             */
            RootWidget(Renderer &renderer_, offset_t w_, offset_t h_): 
                renderer(renderer_), width(w_), height(h_)
            {
            }

            /** Initialize the renderer, then do our own stuff.
             */
            void init()
            {
                renderer.init();
                renderer.define_viewport(0, 0, width, height); // TODO: this may not be the best spot to do this
                bg_color = renderer.rgb_to_native({ 0, 0.5f, 1 });
            }

            // TODO: make into virtual method in Widget base class
            // TODO: parameter for update region
            void repaint() 
            {
                renderer.enter_context();

                // TODO: replace the following dummy
                renderer.fill_rect(0, 0, width, height, bg_color);

                renderer.leave_context();
            }

            // TODO: make this virtual ?, with side-effects (reflow)
            void set_size(int w, int h)
            {
                // TODO
            }

            void key_down(int code)
            {
                // TODO: temporary, for testing only
                if (code == Platform::KeyCodes::ESCAPE) {
                    throw std::runtime_error("Ouch! You've hit my escape key!");
                }
            }

        private:
            Renderer &renderer;
            offset_t width, height;
            typename Renderer::native_color_t bg_color;
        };

    } // ns gui

} // ns gpc