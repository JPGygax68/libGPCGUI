#pragma once

#include <functional>

#include "control.hpp"

namespace gpc {

    namespace gui {

        namespace view_models {
        
            template <
                class Types
            >
            class Button: public Control<Types> {
            public:

                enum class State { UP, DOWN };

                typedef typename Types::point_t point_t;
                typedef std::function<void()> ClickNotifier;
                typedef std::function<void(State)> StateNotifier;

                Button(): _state(State::UP) {}

                void setClickNotifier(ClickNotifier notifier) {
                    click_notifier = notifier;
                }

                void setStateNotifier(StateNotifier notifier) { 
                    state_notifier = notifier;
                }

                State state() const { return _state; }

            private:
                ClickNotifier click_notifier;
                StateNotifier state_notifier;
                State _state;
            };

        } // ns view_models
        
    } // ns gui

} // ns gpc
