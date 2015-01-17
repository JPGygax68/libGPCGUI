#pragma once

#include "widget.hpp"

namespace gpc {

    namespace gui {

        template <class Renderer>
        class Widget {
        public:

            typedef typename Renderer::offset_t offset_t;
            typedef typename Renderer::length_t length_t;

        };
    } // ns gui

} // ns gpc
