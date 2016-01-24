cmake_minimum_required(VERSION 3.1)

project(GPCGUI_All)

enable_testing()

# Force static runtime libraries

if (0)
if (MSVC)
    option(STATICALLY_LINKED_RUNTIMES "Use statically linked runtime libraries" OFF)
    if (STATICALLY_LINKED_RUNTIMES)
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
endif()
 
# add_subdirectory(external/SDL2)

add_subdirectory(external/GPCFontRasterizer/src)

add_subdirectory(external/libGPCGLUtils/lib)

add_subdirectory(external/libGPCGLWrappers/lib)

add_subdirectory(external/libGPCGUIRenderer/lib)

add_subdirectory(external/libGPCGUIGLRenderer/lib)

add_subdirectory(lib)

add_subdirectory(tests)

add_subdirectory(testapp)
