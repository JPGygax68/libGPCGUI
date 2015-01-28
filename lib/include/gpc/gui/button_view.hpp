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
                face_color({ 0.7f, 0.7f, 0.7f }),
                face_hover_color({ 0.8f, 0.8f, 0.8f })
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

            void setFaceColor(const rgba_t &color) {

                face_color = color;

                // TODO: should the former imply the latter ?
                queueResourceUpdate();
                invalidate();
            }

            void setFaceColorHover(const rgba_t &color) {

                face_hover_color = color;

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

                native_colors.face = rend->rgba_to_native(face_color);
                native_colors.light_border = rend->rgba_to_native(interpolate(rgba_t::WHITE(), face_color, 0.25));
                native_colors.shadow_border = rend->rgba_to_native(interpolate(rgba_t::BLACK(), face_color, 0.25));
                native_hover_colors.face = rend->rgba_to_native(face_hover_color);
                native_hover_colors.light_border = rend->rgba_to_native(interpolate(rgba_t::WHITE(), face_hover_color, 0.25));
                native_hover_colors.shadow_border = rend->rgba_to_native(interpolate(rgba_t::BLACK(), face_hover_color, 0.25));

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

            void doRepaint(Renderer *rend, offset_t x_par, offset_t y_par) override {

                ColorSet &colors = isMouseInside() ? native_colors : native_hover_colors;

                // TODO: adapt for "positive up" Y axis
                rend->fill_rect(x_par + x(), y_par + y(), width(), height(), colors.face);
                rend->fill_rect(x_par + x() + 1, y_par + y(), width() - 1, 1, colors.light_border); // top
                rend->fill_rect(x_par + x(), y_par + y() + 1, 1, height() - 2, colors.light_border); // left
                rend->fill_rect(x_par + x() + width() - 1, y_par + y() + 1, 1, height() - 1, colors.shadow_border); // right
                rend->fill_rect(x_par + x() + 1, y_par + y() + height() - 1, width() - 2, 1, colors.shadow_border); // bottom

                if (_reg_font) {

                    // TODO: text color

                    // TODO: support "positive up" Y axis
                    rend->render_text(_reg_font,
                        x_par + x() + caption_origin.x - text_bbox.x_min, y_par + y() + caption_origin.y,
                        _caption.c_str(), _caption.size());
                }
            }

        private:

            struct ColorSet {
                native_color_t face;
                native_color_t light_border, shadow_border;
            };

            ButtonViewModel * vm() { return static_cast<ButtonViewModel*>(this); }

            rast_font_t _rast_font;

            rgba_t face_color, face_hover_color;
            ColorSet native_colors, native_hover_colors;
            reg_font_t _reg_font;
            reg_font_t _unreg_font;
            std::basic_string<char32_t> _caption;
            text_bbox_t text_bbox;
            point_t caption_origin;
            point_t down_where;
        };
            
    } // ns gui
    
} // ns gpc