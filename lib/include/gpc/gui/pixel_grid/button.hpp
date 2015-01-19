#pragma once

#include <gpc/gui/button_view.hpp>

namespace gpc {

    namespace gui {
    
        namespace pixel_grid {
        
            template <
                class InputChannel,
                class Renderer
            >
            class Button : public ButtonView<InputChannel, Renderer> {
            
            public:
                Button(Widget *parent_): ButtonView(parent_) {}
            };
            
        } // ns pixel_grid 
        
    } // ns gui
    
} // ns gpc