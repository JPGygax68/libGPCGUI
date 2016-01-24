project(TestApp)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE libGPCGUI)

# GPC GUI GL Renderer
# Target provided by containing project

target_link_libraries(${PROJECT_NAME} PRIVATE libGPCGUIGLRenderer)

# SDL2
include(FindSDL2)
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARY})

# OpenGL

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})

# glbinding

find_package(glbinding REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLBINDING_STATIC)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLBINDING_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${GLBINDING_LIBRARY_DEBUG}> $<$<CONFIG:Release>:${GLBINDING_LIBRARY_RELEASE}>)
