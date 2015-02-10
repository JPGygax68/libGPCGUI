cmake_minimum_required(VERSION 3.1)

project(libGPCGUI)

add_subdirectory(lib)

add_subdirectory(external/libGPCGLUtils/lib)

add_subdirectory(external/libGPCGLWrappers/lib)

add_subdirectory(external/libGPCGUIRenderer/lib)

add_subdirectory(external/libGPCGUIGLRenderer/lib)

add_subdirectory(testapp)

# Export the targets via the build tree
export(TARGETS libGPCGUI FILE "libGPCGUIRenderer-targets.cmake")
configure_file(project-config.cmake.in "${PROJECT_BINARY_DIR}/libGPCGUI-config.cmake" @ONLY)
export(PACKAGE libGPCGUI)