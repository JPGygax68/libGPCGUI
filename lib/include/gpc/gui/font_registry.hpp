#pragma once

#include <cassert>

namespace gpc {

    namespace gui {

        struct gpc::fonts::RasterizedFont;

        template <class Renderer>
        class FontRegistry {

            typedef const gpc::fonts::RasterizedFont *rast_font_t;
            typedef typename Renderer::reg_font_t reg_font_t;

        public:
            
            FontRegistry(): _rend(nullptr) {}

            ~FontRegistry() {
                assert(_register.empty());
            }

            void setRenderer(Renderer *rend) { 

                assert(!_rend || _rend == rend);

                _rend = rend; 
            }

            void releaseAllFonts() {

                for (auto entry: _register) {
                    _rend->release_font(entry.second);
                }
                _register.clear();
                _reverse.clear();
            }

            auto registerFont(rast_font_t rast_font) -> reg_font_t {

                auto it = _register.find(rast_font);
                if (it != _register.end()) {
                    return it->second;
                }
                else {
                    reg_font_t reg_font = _rend->register_font(*rast_font);
                    _register[rast_font] = reg_font;
                    _reverse[reg_font] = rast_font;
                    return reg_font;
                }
            }

            void releaseFont(reg_font_t reg_font) {

                rast_font_t rast_font = _reverse.find(reg_font)->second;

                _rend->release_font(reg_font);

                _reverse.erase(reg_font);
                _register.erase(rast_font);
            }

        private:
            std::map<rast_font_t, reg_font_t> _register;
            std::map<reg_font_t, rast_font_t> _reverse; // TODO: is this really useful ?
            Renderer *_rend;
        };

    } // ns gui

} // ns gpc
