#pragma once

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

            void init(Renderer *renderer) override {

                color = renderer->rgb_to_native({ 1, 1, 1 });
            }

            void repaint(Renderer *renderer, offset_t x_, offset_t y_) override {

                // TODO: real implementation!
                renderer->fill_rect(x_ + x(), y_ + y(), width(), height(), color);
            }

        private:
            ButtonViewModel * vm() { return static_cast<ButtonViewModel*>(this); }

            point_t down_where;

            // TODO: for testing only, REMOVE!
            typename Renderer::native_color_t color;
        };
            
    } // ns gui
    
} // ns gpc