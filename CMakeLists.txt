cmake_minimum_required(VERSION 3.1)

project(libGPCGUI)

enable_testing()

# Force static runtime libraries

FOREACH(flag
  CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
  CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG_INIT
  CMAKE_CXX_FLAGS_RELEASE  CMAKE_CXX_FLAGS_RELWITHDEBINFO
  CMAKE_CXX_FLAGS_DEBUG  CMAKE_CXX_FLAGS_DEBUG_INIT)
  STRING(REPLACE "/MD"  "/MT" "${flag}" "${${flag}}")
  SET("${flag}" "${${flag}} /EHsc")
ENDFOREACH()
 
add_subdirectory(lib)

add_subdirectory(external/SDL2)

add_subdirectory(external/libGPCGLUtils/lib)

add_subdirectory(external/libGPCGLWrappers/lib)

add_subdirectory(external/libGPCGUIRenderer/lib)

add_subdirectory(external/libGPCGUIGLRenderer/lib)

add_subdirectory(tests)

add_subdirectory(testapp)

# Export the targets via the build tree
export(TARGETS libGPCGUI FILE "libGPCGUIRenderer-targets.cmake")
configure_file(project-config.cmake.in "${PROJECT_BINARY_DIR}/libGPCGUI-config.cmake" @ONLY)
export(PACKAGE libGPCGUI)