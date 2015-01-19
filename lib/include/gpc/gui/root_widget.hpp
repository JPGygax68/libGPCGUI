#pragma once

#include <cstdint>
#include <utility>

#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>

#include <gpc/fonts/RasterizedFont.hpp>
#include <gpc/fonts/cereal.hpp>

#include <gpc/gui/container.hpp>

namespace gpc {

    namespace gui {

        template <class Platform, class Renderer>
        class RootWidget: public Container<Platform, Renderer> {

        public:
            using Container<Platform, Renderer>::offset_t;
            using Container::length_t;
            typedef typename Renderer::font_handle_t font_handle_t;

            RootWidget() {}

            void init(Renderer *renderer) override {

                renderer->define_viewport(0, 0, width(), height()); // TODO: this may not be the best spot to do this
                bg_color = renderer->rgb_to_native({ 0, 0.5f, 1 });

                registerFonts(renderer);

                Container::init(renderer);
            }

            // TODO: make into virtual method in Widget base class
            // TODO: parameter for update region
            void repaint(Renderer *renderer, offset_t x_, offset_t y_) override {

                // TODO: replace the following dummy
                renderer->fill_rect(x_ + x(), y_ + y(), width(), height(), bg_color);

                Container::repaint(renderer, 0, 0);
            }

            // TODO: make this virtual ?, with side-effects (reflow)
            void setSize(int w, int h)
            {
                // TODO
            }

            void keyDown(int code)
            {
                // TODO: temporary, for testing only
                if (code == Platform::KeyCodes::ESCAPE) {
                    throw std::runtime_error("Ouch! You've hit my escape key!");
                }
            }

            auto defaultFont() const -> font_handle_t { return _default_font; }

        private:
            typename Renderer::native_color_t bg_color;

            static auto liberation_sans_regular_16_data() -> std::pair<const uint8_t *, size_t> {

                static uint8_t data[] = {
                    #include "LiberationSans-Regular-16.rft.h"
                };
                return { data, sizeof(data) };
            }

            void registerFonts(Renderer *renderer) {

                _default_font = registerFont(renderer, liberation_sans_regular_16_data());
            }

            auto registerFont(Renderer *renderer, std::pair<const uint8_t *, size_t> data) -> font_handle_t {

                struct membuf : std::streambuf {
                    membuf(char* begin, char* end) {
                        this->setg(begin, begin, end);
                    }
                };

                membuf sbuf((char*)(data.first), (char*)(data.first + data.second));
                std::istream istr(&sbuf);
                cereal::BinaryInputArchive ar(istr);
                gpc::fonts::RasterizedFont rfont;
                ar >> rfont;

                return renderer->register_font(rfont);
            }

            font_handle_t _default_font;
        };

    } // ns gui

} // ns gpc