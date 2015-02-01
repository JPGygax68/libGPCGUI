add_library(libGPCGUI STATIC "src/dummy.cpp" 
  "include/gpc/gui/widget.hpp"
  "include/gpc/gui/font_registry.hpp"
  "include/gpc/gui/container.hpp"
  "include/gpc/gui/root_widget.hpp"
  #"include/gpc/gui/button_viewmodel.hpp"
  "include/gpc/gui/button_view.hpp"
  "include/gpc/gui/pixel_grid/button.hpp"
)

# TODO: use $<INSTALL_INTERFACE:...> and $<BUILD_INTERFACE:...> if the project becomes installable
target_include_directories(libGPCGUI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Embed a font

if (NOT GPCFontRasterizer)
  find_package(GPCFontRasterizer REQUIRED)
endif()

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft
  COMMAND GPCFontRasterizer input="${CMAKE_CURRENT_SOURCE_DIR}/LiberationSans-Regular.ttf" size=16 output="${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft"
)

if (NOT GPCBin2C)
  find_package(GPCBin2C REQUIRED)
endif()

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft.h
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft
  COMMAND GPCBin2C --input=${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft --output=${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft.h
)

target_sources(libGPCGUI PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft.h)

target_include_directories(libGPCGUI PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# GPC Fonts

find_package(libGPCFonts REQUIRED)
target_link_libraries(libGPCGUI PUBLIC libGPCFonts)

# Cereal

# Cereal does not have CMake support afaik
