#pragma once

#include <forward_list>
#include <gpc/gui/widget.hpp>

namespace gpc {

    namespace gui {

        template <class InputChannel, class Renderer>
        class Container: public Widget<InputChannel, Renderer> {
        public:
            //using Widget<InputChannel, Renderer>::offset_t;
            //using Widget::length_t;

            // TODO: Z order parameter ?

            void addChild(Widget *child) {

                _children.push_front(child);
            }

            void removeChild(Widget *child) {

                _children.remove(child);
            }

            void doInit(Renderer *renderer) override {

                for (auto child : _children) {
                    // TODO: init() guards against multiple execution - call doInit() instead ?
                    child->init(renderer);
                }
            }

            void repaint(Renderer *renderer, offset_t x_, offset_t y_) override {

                for (auto child: _children) {
                    child->repaint(renderer, x() + x_, y() + y_);
                }
            }

        private:
            std::forward_list<Widget*> _children;
        };

    } // ns gui

} // ns gpc
