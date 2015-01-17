#pragma once

#include <functional>

namespace gpc {

    namespace gui {

        class ButtonViewModel {
        public:

            enum class state_t { UP, DOWN };

            typedef std::function<void()> ClickNotifier;
            typedef std::function<void(state_t)> StateNotifier;

            ButtonViewModel(): _state(state_t::UP) {}

            void setClickNotifier(ClickNotifier notifier) {
                click_notifier = notifier;
            }

            void setStateNotifier(StateNotifier notifier) { 
                state_notifier = notifier;
            }

            state_t state() const { return _state; }

        private:
            ClickNotifier click_notifier;
            StateNotifier state_notifier;
            state_t _state;
        };

    } // ns gui

} // ns gpc
