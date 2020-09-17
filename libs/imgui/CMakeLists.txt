cmake_minimum_required(VERSION 2.8.12)

set(TARGET_NAME imgui)

set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

project(${TARGET_NAME})

add_compile_options(/MP)

add_library(${TARGET_NAME} STATIC
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    examples/imgui_impl_opengl3.cpp
    examples/imgui_impl_sdl.cpp
)

target_compile_definitions(${TARGET_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW)

target_include_directories( ${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/examples
    ${LIBS_DIR}/SDL/include
    ${LIBS_DIR}/pyxCore/include/ThirdParty/GLEW
)
