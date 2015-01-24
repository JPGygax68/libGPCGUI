#pragma once

namespace gpc {

    namespace gui {

        struct gpc::fonts::RasterizedFont;

        template <class Renderer>
        class FontRegistry {

            // TODO: better names ?
            typedef const gpc::fonts::RasterizedFont *font_t;
            typedef typename Renderer::font_handle_t font_handle_t;

        public:
            
            FontRegistry(): _rend(nullptr) {}

            void setRenderer(Renderer *rend) { 

                assert(!_rend || _rend == rend);

                _rend = rend; 
            }

            auto registerFont(font_t font) -> font_handle_t {

                auto it = _register.find(font);
                if (it != _register.end()) {
                    return it->second;
                }
                else {
                    font_handle_t handle = _rend->register_font(*font);
                    _register[font] = handle;
                    return handle;
                }
            }

            void releaseFont(font_handle_t reg_font) {
            }

        private:
            std::map<font_t, font_handle_t> _register;
            Renderer *_rend;
        };

    } // ns gui

} // ns gpc
