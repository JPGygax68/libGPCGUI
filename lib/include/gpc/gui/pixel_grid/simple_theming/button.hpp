#pragma once

#include <gpc/gui/button_view.hpp>

namespace gpc {

    namespace gui {
    
        namespace pixel_grid {
        
            namespace simple_theming {

                template <
                    class InputChannel,
                    class Renderer // : PixelGridRenderer
                >
                class Button : public ButtonView<InputChannel, Renderer, Button<InputChannel, Renderer> > {
            
                public:
                    Button(Widget *parent_): ButtonView(parent_),
                        _face_color({ 0.7f, 0.7f, 0.7f }),
                        _face_color_hover({ 0.8f, 0.8f, 0.8f })
                        {}

                    void setFaceColor(const rgba_t &color) {

                        _face_color = color;
                        // TODO: should the former imply the latter ?
                        queueResourceUpdate();
                        invalidate();
                    }

                    void setFaceColorHover(const rgba_t &color) {

                        _face_color_hover = color;
                        // TODO: should the former imply the latter ?
                        queueResourceUpdate();
                        invalidate();
                    }

                    void setBorderColor(const rgba_t &color) {

                        _border_color = color;
                        // TODO: should the former imply the latter ?
                        queueResourceUpdate();
                        invalidate();
                    }

                    void setBorderColorHover(const rgba_t &color) {

                        _border_color_hover = color;
                        // TODO: should the former imply the latter ?
                        queueResourceUpdate();
                        invalidate();
                    }

                    void doRepaint(Renderer *rend, offset_t x_par, offset_t y_par) override {

                        rgba_t &color = isMouseInside() ? _face_color : _face_color_hover;
                        rend_color_t native_color = rend->rgba_to_native(color);

                        // Face
                        rend->fill_rect(x_par + x(), y_par + y(), width(), height(), native_color);

                        // Bevel
                        rect_t rect{ x_par + x(), y_par + y(), width(), height() };
                        renderAlphaBevel(rend, rect);
                        rect.grow(-1, -1);
                        renderAlphaBevel(rend, rect);

                        if (captionFont()) {

                            // TODO: text color

                            // TODO: support "positive up" Y axis
                            rend->render_text(captionFont(),
                                x_par + x() + captionOrigin().x - captionBoundingBox().x_min, y_par + y() + captionOrigin().y,
                                captionText().c_str(), captionText().size());
                        }
                    }

                private:
                    rgba_t _border_color, _border_color_hover;
                    rgba_t _face_color, _face_color_hover;
                };

            } // ns simple_theming
            
        } // ns pixel_grid 
        
    } // ns gui
    
} // ns gpc