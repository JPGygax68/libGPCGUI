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
        
            ButtonView(): _font(Renderer::INVALID_FONT_HANDLE), layout_flag(false) {}

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

            void init(Renderer *renderer) override {

                color = renderer->rgb_to_native({ 1, 1, 1 });
            }

            // TODO: is "layout" really the best-fitting term ?
            void updateLayout(Renderer *renderer) {

                if (layout_flag) {
                    auto bbox = renderer->get_text_extents(_font, _caption.c_str(), _caption.size());
                    layout_flag = false;
                }
            }

            void repaint(Renderer *renderer, offset_t x_, offset_t y_) override {

                updateLayout(renderer);

                // TODO: real implementation!
                renderer->fill_rect(x_ + x(), y_ + y(), width(), height(), color);
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

            void invalidateLayout() {
                layout_flag = true;
            }

        private:
            ButtonViewModel * vm() { return static_cast<ButtonViewModel*>(this); }

            font_handle_t _font;
            std::basic_string<char32_t> _caption;

            bool layout_flag;
            point_t down_where;

            // TODO: for testing only, REMOVE!
            typename Renderer::native_color_t color;
        };
            
    } // ns gui
    
} // ns gpc