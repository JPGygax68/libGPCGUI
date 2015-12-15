project(libgpc-gui)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_library(${PROJECT_NAME} STATIC "src/dummy.cpp" 
  "include/gpc/gui/widget.hpp"
  "include/gpc/gui/font_registry.hpp"
  "include/gpc/gui/container.hpp"
  "include/gpc/gui/root_widget.hpp"
  #"include/gpc/gui/button_viewmodel.hpp"
  "include/gpc/gui/button_view.hpp"
  # TODO: the following should probably go to a separate library at some point
  "include/gpc/gui/pixel_grid/simple_theming/button.hpp"
)

# Force static runtime libraries

if (MSVC)
    option(USE_STATICALLY_LINKED_RUNTIMES OFF)
    if (USE_STATICALLY_LINKED_RUNTIMES)
        foreach(flag
          CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
          CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG_INIT
          CMAKE_CXX_FLAGS_RELEASE  CMAKE_CXX_FLAGS_RELWITHDEBINFO
          CMAKE_CXX_FLAGS_DEBUG  CMAKE_CXX_FLAGS_DEBUG_INIT)
          STRING(REPLACE "/MD"  "/MT" "${flag}" "${${flag}}")
          SET("${flag}" "${${flag}} /EHsc")
        endforeach()
    endif()
endif()
 
# TODO: use $<INSTALL_INTERFACE:...> and $<BUILD_INTERFACE:...> if the project becomes installable
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Embed a font

if (NOT TARGET gpc-font-rasterizer)
  find_package(gpc-font-rasterizer REQUIRED)
endif()

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft
  COMMAND GPCFontRasterizer input="${CMAKE_CURRENT_SOURCE_DIR}/LiberationSans-Regular.ttf" size=16 output="${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft"
)

if (NOT TARGET GPCBin2C)
  find_package(GPCBin2C REQUIRED)
endif()

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft.h
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft
  COMMAND GPCBin2C --input=${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft --output=${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft.h
)

target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/LiberationSans-Regular-16.rft.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# GPC GUI Renderer
target_link_libraries(${PROJECT_NAME} PRIVATE libGPCGUIRenderer)

# GPC Fonts
find_package(libGPCFonts REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC libGPCFonts)
