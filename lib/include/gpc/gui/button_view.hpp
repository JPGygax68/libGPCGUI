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
            typedef typename RGBAFloat rgba_t;
            typedef typename gpc::fonts::BoundingBox text_bbox_t;
            typedef typename Widget::point_t point_t;
            typedef typename ButtonViewModel::state_t state_t;
            typedef typename Renderer::native_color_t native_color_t;
        
            ButtonView(Widget *parent_): Widget(parent_),
                //layout_flag(false),
                _rast_font(nullptr),
                _reg_font(0),
                _unreg_font(0),
                bg_color({ 0.7f, 0.7f, 0.7f }),
                bg_color_hover({ 0.8f, 0.8f, 0.8f })
            {}

            template <typename CharT = wchar_t>
            void setCaption(const CharT *text, size_t count = 0U) {

                static_assert(sizeof(CharT) >= 2, "1-byte character set not supported");
            }
            
            void setFont(rast_font_t rast_font) {

                if (rast_font != _rast_font) {
                    if (_reg_font) _unreg_font = _reg_font;
                    _rast_font = rast_font;
                    // TODO: make sure font will be registered with Renderer before next use!
                    _reg_font = 0;
                    updateLayout();
                    queueResourceUpdate();
                }
            }

            void setColor(const rgba_t &color) {

                bg_color = color;

                // TODO: should the former imply the latter ?
                queueResourceUpdate();
                invalidate();
            }

            void setHoverColor(const rgba_t &color) {

                bg_color_hover = color;

                // TODO: should the former imply the latter ?
                queueResourceUpdate();
                invalidate();
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

                updateLayout();
            }

            auto preferredSize() -> area_t override {
                // TODO: add border and padding
                return { text_bbox.width(), text_bbox.height() };
            }

            // TODO: is "layout" really the best-fitting term ?
            void updateLayout() {

                if (_rast_font) {
                    // TODO: support font variants
                    text_bbox = _rast_font->computeTextExtents(0, _caption.c_str(), _caption.size());
                    // TODO: support other alignments than centered
                    caption_origin.x = (width() - text_bbox.width()) / 2;
                    caption_origin.y = (height() - text_bbox.height()) / 2 + text_bbox.y_max;
                }

                invalidate();
            }

            void doUpdateGraphicResources(Renderer *rend, font_registry_t *font_reg) override {

                native_bg_color = rend->rgba_to_native(bg_color);
                native_bg_color_hover = rend->rgba_to_native(bg_color_hover);

                if (_unreg_font) {
                    font_reg->releaseFont(_reg_font);
                    _reg_font = 0;
                }

                if (_rast_font && !_reg_font) {
                    _reg_font = font_reg->registerFont(_rast_font);
                }
                
                //throw std::runtime_error("not implemented yet");
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

            void doInit(Renderer *rend) override {}

            void doRepaint(Renderer *rend, offset_t x_, offset_t y_) override {

                // TODO: should use native colors instead!
                rend->fill_rect(x_ + x(), y_ + y(), width(), height(), isMouseInside() ? native_bg_color_hover : native_bg_color);

                if (_reg_font) {

                    // TODO: text color

                    // TODO: support "positive up" Y axis
                    rend->render_text(_reg_font,
                        x_ + x() + caption_origin.x - text_bbox.x_min, y_ + y() + caption_origin.y,
                        _caption.c_str(), _caption.size());
                }
            }

        private:
            ButtonViewModel * vm() { return static_cast<ButtonViewModel*>(this); }

            rast_font_t _rast_font;
            rgba_t bg_color, bg_color_hover;
            native_color_t native_bg_color, native_bg_color_hover;
            reg_font_t _reg_font;
            reg_font_t _unreg_font;
            std::basic_string<char32_t> _caption;
            text_bbox_t text_bbox;
            point_t caption_origin;
            point_t down_where;
        };
            
    } // ns gui
    
} // ns gpc