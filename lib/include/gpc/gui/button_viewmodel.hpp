#pragma once

#include <functional>

namespace gpc {

    namespace gui {

        class ButtonViewModel {
        public:

            enum class state_t { UP, DOWN };

            typedef std::function<void()> click_notifier_t;
            typedef std::function<void(state_t)> state_notifier_t;

            ButtonViewModel(): _state(state_t::UP) {}

            void setClickNotifier(click_notifier_t notifier) {
                click_notifier = notifier;
            }

            void setStateNotifier(state_notifier_t notifier) { 
                state_notifier = notifier;
            }

            state_t state() const { return _state; }

        private:
            click_notifier_t click_notifier;
            state_notifier_t state_notifier;
            state_t _state;
        };

    } // ns gui

} // ns gpc
