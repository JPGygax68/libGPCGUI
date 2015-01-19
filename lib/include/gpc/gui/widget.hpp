#pragma once

namespace gpc {

    namespace gui {
    
        template <class Platform, class Renderer>
        class Widget {
        public:
            typedef typename Renderer::offset_t offset_t;
            typedef typename Renderer::length_t length_t;
            
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

            Widget(): init_done(false) {}

            // Queries

            auto size() -> size_t const { return _size; }
            auto x() -> offset_t const { return _position.x; }
            auto y() -> offset_t const { return _position.y; }
            auto width() -> length_t const { return _size.w; }
            auto height() -> length_t const { return _size.h; }

            /** The init() method must be called before actual rendering starts. It calls
                the doInit() virtual method exactly once to give the widget a chance to
                allocate resources from / register resources with the renderer.
             */
            virtual void init(Renderer *renderer) {
                
                if (!init_done) {
                    doInit(renderer);
                    init_done = true;
                }
            }

            virtual void repaint(Renderer *renderer, offset_t x, offset_t y) = 0;

            // TODO: should this trigger a reflow or just set the _position & size in one go ?

            void setBounds(const point_t &position_, const area_t &extents_) {
                _position = position_;
                _size = extents_;
            }

        protected:
        
            virtual void doInit(Renderer *renderer) {}

            bool isPointInside(const point_t pt) const {
                return pt.x >= _position.x && pt.x < (_position.x + _size.w)
                    && pt.y >= _position.y && pt.y < (_position.y + _size.h);
            }
            
        private:
            point_t _position;
            area_t _size;
            bool init_done;
        };
            
    } // ns gui
    
} // ns gpc