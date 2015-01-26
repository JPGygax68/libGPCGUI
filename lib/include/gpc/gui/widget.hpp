#pragma once

#include <cassert>

#include "font_registry.hpp"

namespace gpc {

    namespace gui {
    
        template <class Platform, class Renderer>
        class Container;

        template <class Platform, class Renderer>
        class Widget {
        public:
            typedef const gpc::fonts::RasterizedFont *rast_font_t;
            typedef typename FontRegistry<Renderer> font_registry_t;
            typedef typename Container<Platform, Renderer> container_t;
            typedef typename Renderer::offset_t offset_t;
            typedef typename Renderer::length_t length_t;
            typedef typename Renderer::native_color_t rend_color_t;
            typedef typename Renderer::reg_font_t reg_font_t;
            
            typedef std::function<bool(Widget *, int, int)> mouse_enter_handler_t;
            typedef std::function<bool(Widget *, int, int)> mouse_exit_handler_t;

            // TODO: decide whether Point and Extents should be defined by the renderer class instead of here.
            
            struct point_t {
                offset_t x, y;
                bool operator == (const point_t &other) const {
                    return x == other.x && y == other.y;
                }
            };

            struct area_t {
                length_t w, h;
            };

            Widget(Widget *parent_): 
                _parent(parent_), 
                init_done(false),
                must_update_graphic_resources(true),
                mouse_inside(false)
            {}

            // Configuration

            auto addMouseEnterHandler(mouse_enter_handler_t handler) -> mouse_enter_handler_t {

                mouse_enter_handlers.push_back(handler);
                return handler;
            }

            void dropMouseEnterHandlers() {

                mouse_enter_handlers.clear();
            }

            auto addMouseExitHandler(mouse_exit_handler_t handler) -> mouse_exit_handler_t {

                mouse_exit_handlers.push_back(handler);
                return handler;
            }

            void dropMouseExitHandlers() {

                mouse_exit_handlers.clear();
            }

            // Queries

            auto parent() -> Widget * const { return _parent; }
            auto size() -> size_t const { return _size; }
            auto x() -> offset_t const { return _position.x; }
            auto y() -> offset_t const { return _position.y; }
            auto width() -> length_t const { return _size.w; }
            auto height() -> length_t const { return _size.h; }
            bool initialized() const { return init_done; }
            bool isMouseInside() const { return mouse_inside; }

            /** The init() method must be called before actual rendering starts. It calls
                the doInit() virtual method exactly once to give the widget a chance to
                allocate resources from / register resources with the renderer.
             */
            virtual void init(Renderer *rend) {
                
                if (!init_done) {
                    doInit(rend);
                    init_done = true;
                }
            }

            virtual void mouseMotion(int x_, int y_) {

                if (!mouse_inside) {
                    if (isPointInside({x_, y_})) {
                        mouse_inside = true;
                        mouseEnter(x_, y_);
                    }
                }
                else {
                    if (!isPointInside({ x_, y_ })) {
                        mouse_inside = false;
                        mouseExit(x_, y_);
                    }
                }
            }

            void flagForGraphicResourceUpdate() {

                if (!must_update_graphic_resources) {
                    must_update_graphic_resources = true;
                    if (_parent) _parent->flagForGraphicResourceUpdate();
                }
            }

            bool mustUpdateGraphicResources() const { return must_update_graphic_resources; }

            void updateGraphicResources(Renderer *rend, font_registry_t *font_reg) {

                if (must_update_graphic_resources) {
                    doUpdateGraphicResources(rend, font_reg);
                    must_update_graphic_resources = false;
                }
            }

            virtual void repaint(Renderer *renderer, offset_t x, offset_t y) {
                assert(init_done);
            }

            // TODO: should this trigger a reflow or just set the _position & size in one go ?

            virtual auto preferredSize() -> area_t { return _size; }

            void setBounds(const point_t &position_, const area_t &extents_) {
                _position = position_;
                _size = extents_;
            }

        protected:
        
            virtual void doInit(Renderer *rend) {}
            
            virtual void doUpdateGraphicResources(Renderer *rend, font_registry_t *font_reg) {}

            bool isPointInside(const point_t pt) const {
                return pt.x >= _position.x && pt.x < (_position.x + _size.w)
                    && pt.y >= _position.y && pt.y < (_position.y + _size.h);
            }
            
            virtual void mouseEnter(int x_, int y_) {

                for (auto &handler: mouse_enter_handlers) {
                    if (handler(this, x_, y_)) break;
                }
            }

            virtual void mouseExit(int x_, int y_) {

                for (auto &handler : mouse_exit_handlers) {
                    if (handler(this, x_, y_)) break;
                }
            }

        private:
            point_t _position;
            area_t _size;
            Widget *_parent;

            std::vector<mouse_enter_handler_t> mouse_enter_handlers;
            std::vector<mouse_exit_handler_t> mouse_exit_handlers;

            bool init_done;
            bool must_update_graphic_resources;
            bool mouse_inside;
        };
            
    } // ns gui
    
} // ns gpc
