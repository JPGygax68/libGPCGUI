#pragma once

#include <gpc/gui/container.hpp>

namespace gpc {

    namespace gui {

        template <class Platform, class Renderer>
        class RootWidget: public Container<Platform, Renderer> {

        public:
            using Container<Platform, Renderer>::offset_t;
            using Container::length_t;

            RootWidget() {}

            void init(Renderer *renderer) override {

                renderer->define_viewport(0, 0, width(), height()); // TODO: this may not be the best spot to do this
                bg_color = renderer->rgb_to_native({ 0, 0.5f, 1 });

                Container::init(renderer);
            }

            // TODO: make into virtual method in Widget base class
            // TODO: parameter for update region
            void repaint(Renderer *renderer, offset_t x_, offset_t y_) override {

                // TODO: replace the following dummy
                renderer->fill_rect(0, 0, width(), height(), bg_color);

                Container::repaint(renderer, 0, 0);
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
            typename Renderer::native_color_t bg_color;
        };

    } // ns gui

} // ns gpc