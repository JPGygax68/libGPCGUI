#pragma once

#include <cassert>
#include <cstdint>
#include <utility>
#include <map>

#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>

#include <gpc/fonts/RasterizedFont.hpp>
#include <gpc/fonts/cereal.hpp>

#include <gpc/gui/container.hpp>
#include <gpc/gui/font_registry.hpp>

namespace gpc {

    namespace gui {

        template <class Platform, class Renderer>
        class RootWidget: public Container<Platform, Renderer> {

        public:

            RootWidget(): Container(nullptr), _renderer(nullptr), _term_req(false) {

                loadFonts();
            }

            void defineCanvas(Renderer *rend, length_t w, length_t h) {

                _renderer = rend;

                setBounds({0, 0}, {w, h});
                rend->define_viewport(0, 0, width(), height());
                bg_color = rend->rgb_to_native({ 0, 0.5f, 1 });

                _font_registry.setRenderer(rend);

                init(rend);
            }

            bool render() {

                return repaint(_renderer, 0, 0);
            }

            void keyDown(int code)
            {
                // TODO: temporary, for testing only
                if (code == Platform::KeyCodes::ESCAPE) {
                    //throw std::runtime_error("Ouch! You've hit my escape key!");
                    requestTermination();
                }
            }

            void requestTermination() { _term_req = true; }

            bool terminationRequested() { return _term_req; }

            // TODO: we are providing the raw rasterized font; the root widget however could
            // also provide ready-for-use font handles
            auto defaultFont() const -> rast_font_t { return &_default_font; }

            void updateGraphicResources() {

                Widget::updateGraphicResources(_renderer, &_font_registry);
            }

            void releaseGraphicResources() {

                _font_registry.releaseAllFonts();
            }

        protected:

            void doInvalidate() override {

                Platform::pushRepaintEvent();
            }

            // Moving this into protected space, should only be called from render()
            // TODO: parameter for update region
            void doRepaint(Renderer *_renderer, offset_t x_par, offset_t y_par) override {

                _renderer->enter_context();

                // TODO: replace the following dummy
                _renderer->fill_rect(x_par + x(), y_par + y(), width(), height(), bg_color);

                repaintChildren(_renderer, 0, 0);

                _renderer->leave_context();
            }

        private:
            rend_color_t bg_color;

            static auto liberation_sans_regular_16_data() -> std::pair<const uint8_t *, size_t> {

                static uint8_t data[] = {
                    #include "LiberationSans-Regular-16.rft.h"
                };
                return { data, sizeof(data) };
            }

            void loadFonts() {

                _default_font = loadFont(liberation_sans_regular_16_data());
            }

            // TODO: font loading should delegated to an object available (or made available)
            // to all widgets. That object should be managed by the root widget, though
            // in certain cases (like OpenGL under Windows) it could be shared between
            // all displays (and therefore multiple root widgets).

            auto loadFont(std::pair<const uint8_t *, size_t> data) -> gpc::fonts::RasterizedFont {

                struct membuf : std::streambuf {
                    membuf(char* begin, char* end) {
                        this->setg(begin, begin, end);
                    }
                };

                membuf sbuf((char*)(data.first), (char*)(data.first + data.second));
                std::istream istr(&sbuf);
                cereal::BinaryInputArchive ar(istr);
                gpc::fonts::RasterizedFont rast_font;
                ar >> rast_font;

                return rast_font;
            }

            gpc::fonts::RasterizedFont _default_font;
            FontRegistry<Renderer> _font_registry;

            Renderer *_renderer;

            bool _term_req;
        };

    } // ns gui

} // ns gpc