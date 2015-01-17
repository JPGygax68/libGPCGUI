#pragma once

#include "control.hpp"

namespace gpc {

    namespace gui {
    
        namespace views {
        
            template <
                class InputChannel, 
                class Renderer, 
                class ViewModel,
                class ButtonState
            >
            class Button: public Control<InputChannel, Renderer> {
            public:
            
                void pointerButtonDown(const point_t &position) {
                    // TODO: position checking should be (non-virtual) overridable (e.g. bitmap mask)
                    if (isPointInside(position)) {
                        vm()->setState(ButtonState::DOWN);
                        down_where = position;
                    }
                }
                
                void pointerButtonUp(const point_t &position) {
                    vm()->setState(ButtonState::UP);
                    if (position == down_where) {
                        vm()->notifyClick();
                    }
                }

            private:
                ViewModel * vm() { return static_cast<ViewModel*>(this); }

                point_t down_where;
            };
                
        } // ns views
        
    } // ns gui
    
} // ns gpc