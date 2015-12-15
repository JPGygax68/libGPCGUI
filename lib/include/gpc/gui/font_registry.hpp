#pragma once

#include <cassert>

namespace gpc {

    namespace gui {

        struct gpc::fonts::rasterized_font;

        template <class Renderer>
        class font_registry {

            using rasterized_font = gpc::fonts::rasterized_font;
            using font_handle = Renderer::font_handle;

        public:
            
            font_registry(): _rend(nullptr) {}

            ~font_registry() {
                assert(_register.empty());
            }

            void set_renderer(Renderer *rend) { 

                assert(!_rend || _rend == rend);

                _rend = rend; 
            }

            void release_all_fonts() {

                for (auto entry: _register) {
                    _rend->release_font(entry.second);
                }
                _register.clear();
                _reverse.clear();
            }

            auto register_font(const rasterized_font *font) -> registered_font 
            {
                auto it = _register.find(rast_font);
                if (it != _register.end()) {
                    return it->second;
                }
                else {
                    auto handle = _rend->register_font(*font);
                    _register[rast_font] = reg_font;
                    _reverse[reg_font] = rast_font;
                    return reg_font;
                }
            }

            void release_font(registered_font reg_font) 
            {
                rast_font_t rast_font = _reverse.find(reg_font)->second;

                _rend->release_font(reg_font);

                _reverse.erase(reg_font);
                _register.erase(rast_font);
            }

        private:
            std::map<rast_font_t, registered_font> _register;
            std::map<registered_font, rast_font_t> _reverse; // TODO: is this really useful ?
            Renderer *_rend;
        };

    } // ns gui

} // ns gpc
