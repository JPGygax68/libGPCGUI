#pragma once

#include <codecvt>
#include <functional>

#include <gpc/fonts/rasterized_font.hpp>
#include <gpc/fonts/bounding_box.hpp>

#include <gpc/gui/widget.hpp>

namespace gpc {

    namespace gui {
    
        template <
            class Platform, 
            class Renderer,
            class Impl
        >
        class ButtonView: public Widget<Platform, Renderer> {
        public:
            typedef typename gpc::fonts::BoundingBox text_bbox_t;
            typedef typename Widget::point_t point_t;
            typedef typename Renderer::native_color_t native_color_t;
            typedef enum { UP = 0, DOWN = 1} state_t;
            typedef std::function<void(Impl *, state_t)> state_change_handler_t;
        
            ButtonView(Widget *parent_): Widget(parent_),
                //layout_flag(false),
                _rast_font(nullptr),
                _reg_font(0),
                _unreg_font(0),
                _state(UP)
            {}

            template <typename CharT = wchar_t>
            void setCaption(const CharT *text, size_t count = 0U)
            {
                static_assert(sizeof(CharT) >= 2, "1-byte character set not supported");
            }
            
            void setFont(const rasterized_font *font)
            {
                if (font != _font) {
                    if (_reg_font) _unreg_font = _reg_font;
                    _font = font;
                    // TODO: make sure font will be registered with Renderer before next use!
                    _reg_font = 0;
                    updateLayout();
                    queueResourceUpdate();
                }
            }

            template <>
            void setCaption<wchar_t>(const wchar_t *text, size_t count)
            {
                if (count == 0) for (count = 0; text[count] != 0; count++);
                _caption.resize(count);

                auto loc = std::locale("C");
                auto &fac = std::use_facet<std::codecvt<wchar_t, char32_t, std::mbstate_t>>(loc);
                auto mb = std::mbstate_t();
                const wchar_t *from_next;
                char32_t *to_next;
                fac.out(mb, text, text + count, from_next, &_caption[0], &_caption[count], to_next);

                _caption.resize(to_next - &_caption[0]);

                updateLayout();
            }

            void addStateChangeHandler(state_change_handler_t handler)
            {                
                state_change_handlers.push_back(handler);
            }

            void removeStateChangeHandlers()
            {            
                state_change_handlers.clear();
            }

            auto preferredSize() -> area_t override 
            {
                // TODO: add border and padding
                return { text_bbox.width(), text_bbox.height() };
            }

            // TODO: is "layout" really the best-fitting term ?
            void updateLayout()
            {
                if (_rast_font) {
                    // TODO: support font variants
                    text_bbox = _rast_font->computeTextExtents(0, _caption.c_str(), _caption.size());
                    // TODO: support other alignments than centered
                    caption_origin.x = (width() - text_bbox.width()) / 2;
                    caption_origin.y = (height() - text_bbox.height()) / 2 + text_bbox.y_max;
                }

                invalidate();
            }

            void doUpdateGraphicResources(Renderer *rend, font_registry_t *font_reg) override
            {
                if (_unreg_font) {
                    font_reg->releaseFont(_reg_font);
                    _reg_font = 0;
                }

                if (_rast_font && !_reg_font) {
                    _reg_font = font_reg->registerFont(_rast_font);
                }
                
                //throw std::runtime_error("not implemented yet");
            }

            void pointerButtonDown(const point_t &position) 
            {
                // TODO: position checking should be (non-virtual) overridable (e.g. bitmap mask)
                if (isPointInside(position)) {
                    vm()->setState(ButtonState::DOWN);
                    down_where = position;
                }
            }

            void pointerButtonUp(const point_t &position)
            {
                vm()->setState(state_t::UP);
                if (position == down_where) {
                    vm()->notifyClick();
                }
            }

            void mouseButtonDown(int button, int x_par, int y_par) override 
            {
                if (button == 1) {
                    setState(DOWN);
                }

                Widget::mouseButtonDown(button, x_par, y_par);
            }

            void mouseButtonUp(int button, int x_par, int y_par) override
            {
                if (button == 1) {
                    setState(UP);
                }

                Widget::mouseButtonUp(button, x_par, y_par);
            }

        protected:

            void doInit(Renderer *rend) override {}

            auto captionFont() const -> font_handle { return _reg_font; }
            auto captionText() const -> unicode_string { return _caption; }
            auto captionBoundingBox() const -> text_bbox_t { return text_bbox; }
            auto captionOrigin() const -> point_t { return caption_origin; }

        private:

            void setState(state_t state)
            {
                if (state != _state) {
                    for (auto &handler: state_change_handlers) { 
                        handler(static_cast<Impl*>(this), state); };
                    _state = state;
                    invalidate();
                }
            }

            const rasterized_font *_font;

            font_handle _reg_font;
            font_handle _unreg_font;
            unicode_string _caption;
            std::vector<state_change_handler_t> state_change_handlers;
            state_t _state;
            text_bbox_t text_bbox;
            point_t caption_origin;
            point_t down_where;
        };
            
    } // ns gui
    
} // ns gpc