#pragma once

#include <forward_list>
#include <gpc/gui/widget.hpp>

namespace gpc {

    namespace gui {

        template <class InputChannel, class Renderer>
        class Container: public Widget<InputChannel, Renderer> {
        public:

            Container(Widget *parent_): Widget(parent_),
                child_must_update_resources(false)
            {}

            void mouseMotion(int x_abs, int y_abs) override {

                for (auto child: _children) {
                    child->mouseMotion(x_abs - x(), y_abs - y());
                }
            }

            void mouseButtonDown(int button, int x_abs, int y_abs) override {

                int xr = x_abs - x(), yr = y_abs - y();

                for (auto child : _children) {
                    child->mouseButtonDown(button, xr, yr);
                }
            }

            void mouseButtonUp(int button, int x_abs, int y_abs) override {

                int xr = x_abs - x(), yr = y_abs - y();

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

        protected:

            void doRepaint(Renderer *rend, offset_t x_abs, offset_t y_abs) override {

                repaintChildren(rend, x_abs, y_abs);
            }

            void repaintChildren(Renderer *rend, offset_t x_abs, offset_t y_abs) {

                for (auto child : _children) {
                    child->repaint(rend, x_abs, y_abs);
                }
            }

        private:
            std::forward_list<Widget*> _children;
            bool child_must_update_resources;
        };

    } // ns gui

} // ns gpc
