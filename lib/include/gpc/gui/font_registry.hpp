#pragma once

#include <cassert>
#include <map>

namespace gpc {

    namespace fonts {
        struct rasterized_font; // forward declaration
    }

    namespace gui {

        template <class Renderer>
        class font_registry {

            using rasterized_font = gpc::fonts::rasterized_font;
            using font_handle = typename Renderer::font_handle;

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

            auto register_font(const rasterized_font *font) -> font_handle 
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

            void release_font(font_handle handle) 
            {
                const rasterized_font *font = _reverse.find(handle)->second;

                _rend->release_font(handle);

                _reverse.erase(handle);
                _register.erase(font);
            }

        private:
            std::map<const rasterized_font *, font_handle> _register;
            //std::map<font_handle, const rasterized_font *> _reverse; // TODO: is this really useful ?
            Renderer *_rend;
        };

    } // ns gui

} // ns gpc
