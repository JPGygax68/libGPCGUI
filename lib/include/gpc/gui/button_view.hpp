#pragma once

#include <gpc/gui/control_view.hpp>
#include <gpc/gui/button_viewmodel.hpp>

namespace gpc {

    namespace gui {
    
        template <
            class InputChannel, 
            class Renderer
        >
        class ButtonView: public ControlView<InputChannel, Renderer> {
        public:
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

        private:
            ButtonViewModel * vm() { return static_cast<ButtonViewModel*>(this); }

            point_t down_where;
        };
            
    } // ns gui
    
} // ns gpc