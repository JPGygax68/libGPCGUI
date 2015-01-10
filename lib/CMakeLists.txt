add_library(libGPCGUI STATIC "src/dummy.cpp" "include/gpc/gui/gui.hpp")

# TODO: use $<INSTALL_INTERFACE:...> and $<BUILD_INTERFACE:...> if the project becomes installable
target_include_directories(libGPCGUI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
