#pragma once

namespace gpc {

    namespace gui {
    
        template <class InputChannel, class Renderer>
        class ControlView {
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

            struct extents_t {
                length_t w, h;
            };

            // TODO: should this trigger a reflow or just set the position & size in one go ?                

            void setBounds(const point_t &position_, const extents_t &extents_) {
                position = position_;
                extents = extents_;
            }

        protected:
        
            bool isPointInside(const point_t pt) const {
                return pt.x >= position.x && pt.x < (position.x + extents.w)
                    && pt.y >= position.y && pt.y < (position.y + extents.h);
            }
            
        private:
            point_t position;
            extents_t extents;
        };
            
    } // ns gui
    
} // ns gpc