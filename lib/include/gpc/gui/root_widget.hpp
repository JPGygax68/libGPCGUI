#pragma once

#include <cassert>
#include <cstdint>
#include <utility>
#include <map>

#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>

#include <gpc/fonts/rasterized_font.hpp>
#include <gpc/fonts/cereal.hpp>

#include <gpc/gui/container.hpp>
#include <gpc/gui/font_registry.hpp>

namespace gpc {

    namespace gui {

        template <class Platform, class Renderer>
        class RootWidget: public Container<Platform, Renderer> {

        public:

            using platform        = typename Container::platform;
            using renderer        = typename Container::renderer;
            using rasterized_font = typename gpc::fonts::rasterized_font;
            using native_color    = typename Container::native_color;

            RootWidget(): Container(nullptr), _renderer(nullptr), _term_req(false) {

                loadFonts();
            }

            void defineCanvas(renderer *rend, length w, length h, rgba bg_color = {0, 0, 0, 0})
            {
                _renderer = rend;

                setBounds({0, 0}, {w, h});
                rend->define_viewport(0, 0, width(), height());
                rend_bg_color = rend->rgba_to_native(bg_color);

                _font_registry.setRenderer(rend);

                init(rend);
            }

            void setBackgroundColor(const rgba &color)
            {
                _bg_color = renderer::rgba_to_native(color);
                //queueResourceUpdate(); // no longer needed as rgba_to_native is static
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
            auto defaultFont() const -> const rasterized_font * { return &_default_font; }

            void updateGraphicResources() {

                Widget::updateGraphicResources(_renderer, &_font_registry);
            }

            void releaseGraphicResources() {

                _font_registry.releaseAllFonts();
            }

        protected:

            void doInvalidate() override
            {
                Platform::pushRepaintEvent();
            }

            /*
            void doUpdateGraphicResources(renderer *rend, font_registry *font_reg) override
            {
               Container::doUpdateGraphicResources(rend, font_reg);
            }
            */

            // Moving this into protected space, should only be called from render()
            // TODO: parameter for update region
            void doRepaint(renderer *rend, offset x_par, offset y_par) override 
            {
                rend->enter_context();

                // TODO: replace the following dummy
                rend->fill_rect(x_par + x(), y_par + y(), width(), height(), _bg_color);

                repaintChildren(rend, 0, 0);

                rend->leave_context();
            }

        private:

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

            auto loadFont(std::pair<const uint8_t *, size_t> data) -> rasterized_font
            {
                struct membuf : std::streambuf {
                    membuf(char* begin, char* end) {
                        this->setg(begin, begin, end);
                    }
                };

                membuf sbuf((char*)(data.first), (char*)(data.first + data.second));
                std::istream istr(&sbuf);
                cereal::BinaryInputArchive ar(istr);
                gpc::fonts::rasterized_font rast_font;
                ar >> rast_font;

                return rast_font;
            }

            rasterized_font         _default_font;
            font_registry           _font_registry;

            native_color            _bg_color;

            renderer               *_renderer;

            bool                    _term_req;
        };

    } // ns gui

} // ns gpc