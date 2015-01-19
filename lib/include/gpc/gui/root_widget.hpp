#pragma once

#include <cassert>
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
            typedef typename Renderer::offset_t offset_t;
            typedef typename Renderer::length_t length_t;
            typedef typename Renderer::font_handle_t font_handle_t;

            RootWidget(): Container(nullptr), _renderer(nullptr) {}

            void setCanvas(Renderer *rend, length_t w, length_t h) {

                _renderer = rend;
                setBounds({0, 0}, {w, h});
                rend->define_viewport(0, 0, width(), height());
                bg_color = rend->rgb_to_native({ 0, 0.5f, 1 });
                registerFonts(rend);
                init(rend);
            }

            void render() {

                _renderer->enter_context();
                repaint(_renderer, 0, 0);
                _renderer->leave_context();
            }

            void keyDown(int code)
            {
                // TODO: temporary, for testing only
                if (code == Platform::KeyCodes::ESCAPE) {
                    throw std::runtime_error("Ouch! You've hit my escape key!");
                }
            }

            auto defaultFont() const -> font_handle_t { assert(initialized()); return _default_font; }

        protected:

            // Moving this into protected space, should only be called from render()
            // TODO: parameter for update region
            void repaint(Renderer *_renderer, offset_t x_, offset_t y_) override {

                // TODO: replace the following dummy
                _renderer->fill_rect(x_ + x(), y_ + y(), width(), height(), bg_color);

                Container::repaint(_renderer, 0, 0);
            }

        private:
            typename Renderer::native_color_t bg_color;

            static auto liberation_sans_regular_16_data() -> std::pair<const uint8_t *, size_t> {

                static uint8_t data[] = {
                    #include "LiberationSans-Regular-16.rft.h"
                };
                return { data, sizeof(data) };
            }

            void registerFonts(Renderer *_renderer) {

                _default_font = registerFont(_renderer, liberation_sans_regular_16_data());
            }

            auto registerFont(Renderer *_renderer, std::pair<const uint8_t *, size_t> data) -> font_handle_t {

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

                return _renderer->register_font(rfont);
            }

            font_handle_t _default_font;
            Renderer *_renderer;
        };

    } // ns gui

} // ns gpc