#pragma once

namespace gpc {

    namespace gui {

        template <class Renderer>
        class RootWidget {

        public:

            RootWidget(Renderer &renderer_, int w_, int h_): 
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

            // TODO: make into virtual method
            // TODO: parameter for update region
            void repaint() 
            {
                renderer.enter_context();

                // TODO: replace the following dummy
                renderer.fill_rect(0, 0, width, height, bg_color);

                renderer.leave_context();
            }

            // TODO: make this virtual ?, with side-effects (reflow)
            void setSize(int w, int h)
            {
                // TODO
            }

        private:
            Renderer &renderer;
            int width, height;
            typename Renderer::native_color_t bg_color;
        };

    } // ns gui

} // ns gpc