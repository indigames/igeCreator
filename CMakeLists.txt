cmake_minimum_required(VERSION 3.4.1)

set(TARGET_NAME igeCreator)
file(TO_CMAKE_PATH $ENV{IGE_LIBS} IGE_LIBS)

project(${TARGET_NAME})

set (CMAKE_CXX_STANDARD 17)
set(IGE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(APP_STYLE STATIC)

add_compile_definitions(EDITOR_MODE=1)

add_subdirectory(${IGE_ROOT}/pyxCore/pyxcore  ${CMAKE_BINARY_DIR}/pyxcore)
add_subdirectory(${IGE_ROOT}/igeCore  ${CMAKE_BINARY_DIR}/igeCore)
add_subdirectory(${IGE_ROOT}/igeVMath  ${CMAKE_BINARY_DIR}/igeVMath)
add_subdirectory(${IGE_ROOT}/igePAL  ${CMAKE_BINARY_DIR}/igePAL)
add_subdirectory(${IGE_ROOT}/igeScene  ${CMAKE_BINARY_DIR}/igeScene)
add_subdirectory(${IGE_ROOT}/imgui  ${CMAKE_BINARY_DIR}/imgui)
add_subdirectory(${IGE_ROOT}/SDL  ${CMAKE_BINARY_DIR}/SDL)

file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(${TARGET_NAME} WIN32 ${SRC_FILES})

target_link_libraries(${TARGET_NAME}
    pyxcore-static
    igeCore-static
    igeVMath-static
    igePAL-static
    igeScene-static
    imgui
    SDL2-static
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${IGE_ROOT}/pyxCore/pyxcore
    ${IGE_ROOT}/pyxCore/ThirdParty/GLEW
    ${IGE_ROOT}/pyxCore/ThirdParty/edge/include
    ${IGE_ROOT}/pyxCore/ThirdParty/spdlog/include
    ${IGE_ROOT}/tracy
    ${IGE_ROOT}/igeLibs/cpp-taskflow/include
    ${IGE_ROOT}/igeLibs/json/include
    ${IGE_ROOT}/igeVmath
    ${IGE_ROOT}/igeCore
    ${IGE_ROOT}/igePAL
    ${IGE_ROOT}/igeScene
    ${IGE_ROOT}/imgui
    ${IGE_ROOT}/SDL/include
    ${IGE_ROOT}/stb
)

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/app"
)
