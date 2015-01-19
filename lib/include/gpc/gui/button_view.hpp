#pragma once

#include <codecvt>

#include <gpc/gui/widget.hpp>
#include <gpc/gui/button_viewmodel.hpp>

namespace gpc {

    namespace gui {
    
        template <
            class Platform, 
            class Renderer
        >
        class ButtonView: public Widget<Platform, Renderer> {
        public:
            typedef typename Widget::point_t point_t;
            typedef typename ButtonViewModel::state_t state_t;
            typedef typename Renderer::font_handle_t font_handle_t;
            typedef typename Renderer::text_bbox_t text_bbox_t;
            typedef typename Renderer::native_color_t color_t;
        
            ButtonView(Widget *parent_): Widget(parent_),
                _font(Renderer::INVALID_FONT_HANDLE), layout_flag(false) 
            {}

            template <typename CharT = wchar_t>
            void setCaption(const CharT *text, size_t count = 0U) {

                static_assert(sizeof(CharT) >= 2, "1-byte character set not supported");
            }
            
            void setFont(font_handle_t font) {

                if (font != _font) {
                    _font = font;
                    invalidateLayout();
                }
            }

            template <>
            void setCaption<wchar_t>(const wchar_t *text, size_t count) {

                if (count == 0) for (count = 0; text[count] != 0; count++);
                _caption.resize(count);

                auto loc = std::locale("C");
                auto &fac = std::use_facet<std::codecvt<wchar_t, char32_t, std::mbstate_t>>(loc);
                auto mb = std::mbstate_t();
                const wchar_t *from_next;
                char32_t *to_next;
                fac.out(mb, text, text + count, from_next, &_caption[0], &_caption[count], to_next);

                _caption.resize(to_next - &_caption[0]);

                invalidateLayout();
            }

            // TODO: is "layout" really the best-fitting term ?
            void updateLayout(Renderer *rend) {

                init(rend);

                if (layout_flag) {
                    text_bbox = rend->get_text_extents(_font, _caption.c_str(), _caption.size());
                    // TODO: support other alignments than centered
                    caption_origin.x = (width() - text_bbox.width()) / 2;
                    caption_origin.y = (height() - text_bbox.height()) / 2 + text_bbox.y_max;
                    layout_flag = false;
                }
            }

            void repaint(Renderer *rend, offset_t x_, offset_t y_) override {

                updateLayout(rend);

                rend->fill_rect(x_ + x(), y_ + y(), width(), height(), bg_color);

                // TODO: text color

                // TODO: support "positive up" Y axis
                rend->render_text(_font, 
                    x_ + x() + caption_origin.x - text_bbox.x_min, y_ + y() + caption_origin.y, 
                    _caption.c_str(), _caption.size());
            }

            void pointerButtonDown(const point_t &position) {

                // TODO: position checking should be (non-virtual) overridable (e.g. bitmap mask)
                if (isPointInside(position)) {
                    vm()->setState(ButtonState::DOWN);
                    down_where = position;
                }
            }

            void pointerButtonUp(const point_t &position) {

                vm()->setState(state_t::UP);
                if (position == down_where) {
                    vm()->notifyClick();
                }
            }

        protected:

            void doInit(Renderer *rend) override {

                bg_color = rend->rgb_to_native({ 1, 1, 1 });
            }

            void invalidateLayout() {
                layout_flag = true;
            }

        private:
            ButtonViewModel * vm() { return static_cast<ButtonViewModel*>(this); }

            color_t bg_color;
            font_handle_t _font;
            std::basic_string<char32_t> _caption;
            text_bbox_t text_bbox;
            point_t caption_origin;
            bool layout_flag;
            point_t down_where;
        };
            
    } // ns gui
    
} // ns gpc