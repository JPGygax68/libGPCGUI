#pragma once

#include <gpc/gui/renderer.hpp>
#include <gpc/gui/button_view.hpp>

#include <gpc/gui/property_set.hpp>

namespace gpc {

    namespace gui {
    
        namespace pixel_grid {
        
            namespace simple_theming {

                template <
                    class InputChannel,
                    class Renderer /* : PixelGridRenderer */
                >
                class Button : public ButtonView<InputChannel, Renderer, Button<InputChannel, Renderer> > {
            
                public:
                    Button(Widget *parent_): ButtonView(parent_),
                        _rgba_colors({ 
                            { 0.0f, 0.3f, 0.5f }, { 0.0f, 0.3f, 0.6f },
                            { 0.8f, 0.8f, 0.8f }, { 0.9f, 0.9f, 0.9f }
                        })
                    {}

                    void setFaceColor(const rgba_t &color) {

                        _rgba_colors.face = color;
                        // TODO: should the former imply the latter ?
                        queueResourceUpdate();
                        invalidate();
                    }

                    void setFaceColorHover(const rgba_t &color) {

                        _rgba_colors.face_hover = color;
                        // TODO: should the former imply the latter ?
                        queueResourceUpdate();
                        invalidate();
                    }

                    void setBorderColor(const rgba_t &color) {

                        _rgba_colors.border = color;
                        // TODO: should the former imply the latter ?
                        queueResourceUpdate();
                        invalidate();
                    }

                    void setBorderColorHover(const rgba_t &color) {

                        _rgba_colors.border_hover = color;
                        // TODO: should the former imply the latter ?
                        queueResourceUpdate();
                        invalidate();
                    }

                    void setCaptionColor(const rgba_t &color) {

                        _rgba_colors.caption = color;
                        queueResourceUpdate();
                    }

                protected:

                    // TODO: is it appropriate to reuse the same virtual method for theming, or
                    // should a specialized method be introduced (which probably would not need
                    // access to the font registry) ?
                    void doUpdateGraphicResources(Renderer *rend, font_registry_t *font_reg) override {

                        _native_colors.face         = rend->rgba_to_native(_rgba_colors.face);
                        _native_colors.face_hover   = rend->rgba_to_native(_rgba_colors.face_hover);
                        _native_colors.border       = rend->rgba_to_native(_rgba_colors.border);
                        _native_colors.border_hover = rend->rgba_to_native(_rgba_colors.border_hover);
                        _native_colors.caption      = rend->rgba_to_native(_rgba_colors.caption);

                        ButtonView::doUpdateGraphicResources(rend, font_reg);
                    }

                    void doRepaint(Renderer *rend, offset_t x_par, offset_t y_par) override {

                        auto native_face   = isMouseInside() ? _native_colors.face   : _native_colors.face_hover;
                        auto native_border = isMouseInside() ? _native_colors.border : _native_colors.border_hover;

                        auto x_ = x_par + x(), y_ = y_par + y();
                        auto w_ = width(), h_ = height();

                        // Face
                        rend->fill_rect(x_, y_, w_, h_, native_face);

                        // Border
                        rend->fill_rect(x_ + 1, y_, w_ - 1, 1, native_border); // top
                        rend->fill_rect(x_, y_, 1, h_, native_border); // left
                        rend->fill_rect(x_ + w_ - 1, y_ + 1, 1, h_ - 1, native_border); // right
                        rend->fill_rect(x_ + 1, y_ + h_ - 1, w_ - 2, 1, native_border); // bottom
                        /*
                        rect_t rect{ x_par + x(), y_par + y(), width(), height() };
                        renderAlphaBevel(rend, rect);
                        rect.grow(-1, -1);
                        renderAlphaBevel(rend, rect);
                        */

                        if (captionFont()) {

                            // TODO: text color
                            rend->set_text_color(_native_colors.caption);

                            // TODO: support "positive up" Y axis
                            rend->render_text(captionFont(),
                                x_par + x() + captionOrigin().x - captionBoundingBox().x_min, y_par + y() + captionOrigin().y,
                                captionText().c_str(), captionText().size());
                        }
                    }

                private:
                    template <typename Color> struct Colors {
                        Color border, border_hover;
                        Color face, face_hover;
                        Color caption;
                    };
                    Colors<rgba_t> _rgba_colors;
                    Colors<native_color_t> _native_colors;
                };

            } // ns simple_theming
            
        } // ns pixel_grid 
        
    } // ns gui
    
} // ns gpc