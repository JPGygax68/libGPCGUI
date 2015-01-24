#pragma once

#include <codecvt>

#include <gpc/fonts/RasterizedFont.hpp>
#include <gpc/fonts/BoundingBox.hpp>

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
            typedef typename const gpc::fonts::RasterizedFont *font_t;
            typedef typename gpc::fonts::BoundingBox text_bbox_t;
            typedef typename Widget::point_t point_t;
            typedef typename ButtonViewModel::state_t state_t;
            typedef typename Renderer::font_handle_t font_handle_t;
            typedef typename Renderer::native_color_t color_t;
        
            ButtonView(Widget *parent_): Widget(parent_),
                layout_flag(false),
                _rast_font(nullptr),
                _reg_font(Renderer::INVALID_FONT_HANDLE),
                _unreg_font(Renderer::INVALID_FONT_HANDLE) 
            {}

            template <typename CharT = wchar_t>
            void setCaption(const CharT *text, size_t count = 0U) {

                static_assert(sizeof(CharT) >= 2, "1-byte character set not supported");
            }
            
            void setFont(font_t rast_font) {

                if (rast_font != _rast_font) {
                    if (_reg_font) _unreg_font = _reg_font;
                    _rast_font = rast_font;
                    // TODO: make sure font will be registered with Renderer before next use!
                    _reg_font = Renderer::INVALID_FONT_HANDLE;
                    flagForGraphicResourceUpdate();
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

            auto preferredSize() -> area_t override {
                // TODO: add border and padding
                return { text_bbox.width(), text_bbox.height() };
            }

            // TODO: is "layout" really the best-fitting term ?
            void updateLayout(Renderer *rend) {

                init(rend);

                if (layout_flag) {
                    if (_rast_font) {
                        // TODO: support font variants
                        text_bbox = _rast_font->computeTextExtents(0, _caption.c_str(), _caption.size());
                        // TODO: support other alignments than centered
                        caption_origin.x = (width() - text_bbox.width()) / 2;
                        caption_origin.y = (height() - text_bbox.height()) / 2 + text_bbox.y_max;
                    }
                    layout_flag = false;
                }
            }

            void doUpdateGraphicResources(font_registry_t *font_reg) override {

                if (_unreg_font) {
                    font_reg->releaseFont(_reg_font);
                    _reg_font = Renderer::INVALID_FONT_HANDLE;
                }

                if (_rast_font && (_reg_font == Renderer::INVALID_FONT_HANDLE)) {
                    _reg_font = font_reg->registerFont(_rast_font);
                }
                
                //throw std::runtime_error("not implemented yet");
            }

            void repaint(Renderer *rend, offset_t x_, offset_t y_) override {

                updateLayout(rend);

                rend->fill_rect(x_ + x(), y_ + y(), width(), height(), bg_color);

                if (_reg_font != Renderer::INVALID_FONT_HANDLE) {

                    // TODO: text color

                    // TODO: support "positive up" Y axis
                    rend->render_text(_reg_font, 
                        x_ + x() + caption_origin.x - text_bbox.x_min, y_ + y() + caption_origin.y, 
                        _caption.c_str(), _caption.size());
                }
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

            font_t _rast_font;
            color_t bg_color;
            font_handle_t _reg_font;
            font_handle_t _unreg_font;
            std::basic_string<char32_t> _caption;
            text_bbox_t text_bbox;
            point_t caption_origin;
            bool layout_flag;
            point_t down_where;
        };
            
    } // ns gui
    
} // ns gpc