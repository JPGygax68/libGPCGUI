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

            Container(Widget *parent_): Widget(parent_),
                child_must_update_resources(false)
            {}

            void mouseMotion(int x_, int y_) override {

                for (auto child: _children) {
                    child->mouseMotion(x_ - x(), y_ - y());
                }
            }

            void mouseButtonDown(int button, int x_, int y_) override {

                int xr = x_ - x(), yr = y_ - y();

                for (auto child : _children) {
                    child->mouseButtonDown(button, xr, yr);
                }
            }

            void mouseButtonUp(int button, int x_, int y_) override {

                int xr = x_ - x(), yr = y_ - y();

                for (auto child : _children) {
                    child->mouseButtonUp(button, xr, yr);
                }
            }

            // TODO: Z order parameter ?

            void addChild(Widget *child) {

                _children.push_front(child);
            }

            void removeChild(Widget *child) {

                _children.remove(child);
            }

            void doInit(Renderer *rend) override {

                for (auto child : _children) {
                    // TODO: init() guards against multiple execution - call doInit() instead ?
                    child->init(rend);
                }
            }

            /** Override this if your Container-derived widget needs graphics resources of
                its own, and do not forget to call updateChildrenGraphicResources().
             */
            void doUpdateGraphicResources(Renderer *rend, font_registry_t *font_reg) override {

                updateChildrenGraphicResources(rend, font_reg);
            }

            void updateChildrenGraphicResources(Renderer *rend, font_registry_t *font_reg) {

                for (auto child: _children) {
                    if (child->mustUpdateGraphicResources()) {
                        child->updateGraphicResources(rend, font_reg);
                    }
                }
            }

            void repaint(Renderer *rend, offset_t x_, offset_t y_) override {

                Widget::repaint(rend, x_, x_);

                for (auto child: _children) {
                    child->repaint(rend, x_, y_);
                }
            }

        private:
            std::forward_list<Widget*> _children;
            bool child_must_update_resources;
        };

    } // ns gui

} // ns gpc
