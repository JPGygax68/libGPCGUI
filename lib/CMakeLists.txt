add_library(libGPCGUI STATIC "src/dummy.cpp" 
  "include/gpc/gui/widget.hpp"
  "include/gpc/gui/container.hpp"
  "include/gpc/gui/root_widget.hpp"
  "include/gpc/gui/button_viewmodel.hpp"
  "include/gpc/gui/button_view.hpp"
  "include/gpc/gui/pixel_grid/button.hpp"
)

# TODO: use $<INSTALL_INTERFACE:...> and $<BUILD_INTERFACE:...> if the project becomes installable
target_include_directories(libGPCGUI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
