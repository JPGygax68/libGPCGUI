#pragma once

#include <cassert>

#include <gpc/gui/renderer.hpp>

#include "font_registry.hpp"

namespace gpc {

    namespace gui {
    
        template <class Platform, class Renderer>
        class Container;

        template <class Platform, class Renderer>
        class Widget {
        public:
            typedef typename RGBAFloat rgba_t;
            typedef Renderer renderer_t;
            typedef const gpc::fonts::RasterizedFont *rast_font_t;
            typedef typename FontRegistry<Renderer> font_registry_t;
            typedef typename Container<Platform, Renderer> container_t;
            typedef typename Renderer::offset_t offset_t;
            typedef typename Renderer::length_t length_t;
            typedef typename Renderer::native_color_t rend_color_t;
            typedef typename Renderer::reg_font_t reg_font_t;
            
            typedef std::function<bool(Widget *, int, int)> mouse_enter_handler_t;
            typedef std::function<bool(Widget *, int, int)> mouse_exit_handler_t;
            typedef std::function<bool(Widget *, int, int, int)> mouse_button_down_handler_t;
            typedef std::function<bool(Widget *, int, int, int)> mouse_button_up_handler_t;
            typedef std::function<bool(Widget *, int, int, int)> mouse_click_handler_t;

            // TODO: decide whether Point and Extents should be defined by the renderer class instead of here.
            
            struct point_t {
                offset_t x, y;
                bool operator == (const point_t &other) const {
                    return x == other.x && y == other.y;
                }
                auto operator + (const point_t &other) const -> point_t {
                    return { x + other.x, y + other.y };
                }
            };

            struct area_t {
                length_t w, h;
            };

            struct rect_t {
                point_t position;
                area_t size;
                offset_t x() const { return position.x; }
                offset_t y() const { return position.y; }
                offset_t w() const { return size.w; }
                offset_t h() const { return size.h; }
                auto operator + (const point_t &offs) const -> rect_t {
                    return { position + offs, size };
                }
                auto grow(offset_t dx = 1, offset_t dy = 1) -> rect_t & {
                    position.x -= dx, position.y -= dx;
                    size.w += 2 * dx, size.h += 2 * dy;
                    return *this;
                }
            };

            Widget(Widget *parent_): 
                _parent(parent_), 
                init_done(false),
                must_update_graphic_resources(true),
                must_repaint(true),
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

            auto addMouseButtonDownHandler(mouse_button_down_handler_t handler) -> mouse_button_down_handler_t {

                mouse_button_down_handlers.push_back(handler);
                return handler;
            }

            void dropMouseButtonDownHandlers() {

                mouse_button_down_handlers.clear();
            }

            auto addMouseButtonUpHandler(mouse_button_up_handler_t handler) -> mouse_button_up_handler_t {

                mouse_button_up_handlers.push_back(handler);
                return handler;
            }

            void dropMouseButtonUpHandlers() {

                mouse_button_up_handlers.clear();
            }

            auto addMouseClickHandler(mouse_click_handler_t handler) -> mouse_click_handler_t {

                mouse_click_handlers.push_back(handler);
                return handler;
            }

            void dropMouseClickHandlers() {

                mouse_click_handlers.clear();
            }

            // Queries

            auto parent() -> Widget * const { return _parent; }
            auto size() -> size_t const { return _size; }
            auto x() -> offset_t const { return _position.x; }
            auto y() -> offset_t const { return _position.y; }
            auto position() const -> point_t { return _position; }
            auto width() const -> length_t { return _size.w; }
            auto height() const -> length_t { return _size.h; }
            auto size() const -> area_t { return _size; }
            bool initialized() const { return init_done; }
            bool isMouseInside() const { return mouse_inside; }
            bool mustUpdateGraphicResources() const { return must_update_graphic_resources; }
            bool mustRepaint() const { return Platform::NEEDS_FULL_REDRAWS || must_repaint; }

            /** The init() method must be called before actual rendering starts. It calls
                the doInit() virtual method exactly once to give the widget a chance to
                allocate resources from / register resources with the renderer.
                TODO: still necessary now that there is updateGraphicResources() e.a. ?
             */
            virtual void init(Renderer *rend) {
                
                if (!init_done) {
                    doInit(rend);
                    init_done = true;
                }
            }

            virtual void mouseMotion(int x_par, int y_par) {

                if (!mouse_inside) {
                    if (isPointInside({x_par, y_par})) {
                        mouse_inside = true;
                        mouseEnter(x_par, y_par);
                    }
                }
                else {
                    if (!isPointInside({ x_par, y_par })) {
                        mouse_inside = false;
                        mouseExit(x_par, y_par);
                    }
                }
            }

            virtual void mouseButtonDown(int button, int x_par, int y_par) {

                if (isPointInside({x_par, y_par})) {
                    mouse_down_point = {x_par, y_par};
                    mouse_down_button = button;
                }
            }

            virtual void mouseButtonUp(int button, int x_par, int y_par) {

                if (x_par >= (mouse_down_point.x - 1) && x_par < (mouse_down_point.x + 1) &&
                    y_par >= (mouse_down_point.y - 1) && y_par < (mouse_down_point.y + 1) &&
                    mouse_down_button == button)
                {
                    mouseClick(button, x_par, y_par);
                }
            }

            void queueResourceUpdate() {

                if (!must_update_graphic_resources) {
                    must_update_graphic_resources = true;
                    if (_parent) _parent->queueResourceUpdate();
                }
            }

            void updateGraphicResources(Renderer *rend, font_registry_t *font_reg) {

                if (must_update_graphic_resources) {
                    doUpdateGraphicResources(rend, font_reg);
                    must_update_graphic_resources = false;
                }
            }

            void invalidate() {

                if (!must_repaint) {
                    must_repaint = true;
                    if (_parent) _parent->invalidate();
                    doInvalidate();
                }
            }

            bool repaint(Renderer *rend, offset_t x_par, offset_t y_par) {
                
                assert(init_done);

                if (mustRepaint()) { 
                    doRepaint(rend, x_par, y_par);
                    must_repaint = false;
                    return true;
                }
                else 
                    return false;
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

            virtual void doInvalidate() {}

            virtual void doRepaint(Renderer *rend, offset_t x_par, offset_t y_par) = 0;

            bool isPointInside(const point_t pt) const {
                return pt.x >= _position.x && pt.x < (_position.x + _size.w)
                    && pt.y >= _position.y && pt.y < (_position.y + _size.h);
            }
            
            virtual void mouseEnter(int x_par, int y_par) {

                for (auto &handler: mouse_enter_handlers) {
                    if (handler(this, x_par, y_par)) return; // this prevents automatic invalidation
                }

                // TODO: make this dependent upon a flag ("invalidate on hover") ?
                invalidate();
            }

            virtual void mouseExit(int x_par, int y_par) {

                for (auto &handler : mouse_exit_handlers) {
                    if (handler(this, x_par, y_par)) return; // this prevents automatic invalidation
                }

                // TODO: make this dependent upon a flag ("invalidate on hover") ?
                invalidate();
            }

            virtual void mouseClick(int button, int x_par, int y_par) {

                for (auto &handler : mouse_click_handlers) {
                    if (handler(this, button, x_par, y_par)) break;
                }
            }

            void renderAlphaBevel(Renderer *rend, const rect_t &rect) {
                
                offset_t x = rect.x(), y = rect.y();
                length_t w = rect.w(), h = rect.h();

                renderAlphaBevel(rend, x, y, w, h);
            }

            void renderAlphaBevel(Renderer *rend, offset_t x, offset_t y, length_t w, length_t h) {

                // TODO: make alpha value configurable
                static const rend_color_t light  = Renderer::rgba_to_native({1, 1, 1, 0.75f});
                static const rend_color_t shadow = Renderer::rgba_to_native({0, 0, 0, 0.75f });

                // TODO: adapt for "positive up" Y axis
                rend->fill_rect(x + 1    , y        , w - 1, 1    , light ); // top
                rend->fill_rect(x        , y        , 1    , h    , light ); // left
                rend->fill_rect(x + w - 1, y + 1    , 1    , h - 1, shadow); // right
                rend->fill_rect(x + 1    , y + h - 1, w - 2, 1    , shadow); // bottom
            }

        private:
            point_t _position;
            area_t _size;
            Widget *_parent;

            std::vector<mouse_enter_handler_t> mouse_enter_handlers;
            std::vector<mouse_exit_handler_t> mouse_exit_handlers;
            std::vector<mouse_button_down_handler_t> mouse_button_down_handlers;
            std::vector<mouse_button_up_handler_t> mouse_button_up_handlers;
            std::vector<mouse_click_handler_t> mouse_click_handlers;

            bool init_done;
            bool must_update_graphic_resources;
            bool must_repaint;
            bool mouse_inside;
            point_t mouse_down_point;
            int mouse_down_button;
        };
            
    } // ns gui
    
} // ns gpc
