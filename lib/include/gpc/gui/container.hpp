#pragma once

#include <forward_list>
#include <gpc/gui/widget.hpp>

namespace gpc {

    namespace gui {

        template <class InputChannel, class Renderer>
        class Container: public Widget<InputChannel, Renderer> {
        public:

            using offset = typename Widget::offset;

            Container(Widget *parent_): Widget(parent_),
                child_must_update_resources(false)
            {}

            void mouseMotion(int x_par, int y_par) override {

                for (auto child: _children) {
                    child->mouseMotion(x_par - x(), y_par - y());
                }
            }

            void mouseButtonDown(int button, int x_par, int y_par) override {

                int xr = x_par - x(), yr = y_par - y();

                for (auto child : _children) {
                    child->mouseButtonDown(button, xr, yr);
                }
            }

            void mouseButtonUp(int button, int x_par, int y_par) override {

                int xr = x_par - x(), yr = y_par - y();

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
            void doUpdateGraphicResources(renderer *rend, font_registry *font_reg) override
            {
                updateChildrenGraphicResources(rend, font_reg);
            }

            void updateChildrenGraphicResources(renderer *rend, font_registry *font_reg)
            {
                for (auto child: _children) {
                    if (child->mustUpdateGraphicResources()) {
                        child->updateGraphicResources(rend, font_reg);
                    }
                }
            }

        protected:

            void doRepaint(renderer *rend, offset x_par, offset y_par) override {

                repaintChildren(rend, x_par, y_par);
            }

            void repaintChildren(renderer *rend, offset x_par, offset y_par) {

                for (auto child : _children) {
                    child->repaint(rend, x_par + x(), y_par + y());
                }
            }

        private:
            std::forward_list<Widget*> _children;
            bool child_must_update_resources;
        };

    } // ns gui

} // ns gpc
