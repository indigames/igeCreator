cmake_minimum_required(VERSION 3.6)

set(TARGET_NAME igeCreator)

project(${TARGET_NAME})

set(USE_MSVC_RELEASE_RUNTIME_ALWAYS TRUE)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(/MP /std:c++17)

# [IGE]: conan build config
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set(CONAN_INCLUDE_DIRS_PYXCORE "")
    set(CONAN_LIBS_PYXCORE "")
    set(CONAN_INCLUDE_DIRS_IGEVMATH "")
    set(CONAN_LIBS_IGEVMATH "")
    conan_basic_setup()
endif()
# [/IGE]

IF (USE_MSVC_RELEASE_RUNTIME_ALWAYS)
    FOREACH(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
        IF(${flag_var} MATCHES "/MDd")
            STRING(REGEX REPLACE "/MDd" "/MD" ${flag_var} "${${flag_var}}")
        ENDIF(${flag_var} MATCHES "/MDd")
        IF(${flag_var} MATCHES "/MTd")
            STRING(REGEX REPLACE "/MTd" "/MT" ${flag_var} "${${flag_var}}")
        ENDIF(${flag_var} MATCHES "/MTd")
        IF(${flag_var} MATCHES "-D_DEBUG")
            STRING(REGEX REPLACE "-D_DEBUG" "" ${flag_var} "${${flag_var}}")
        ENDIF(${flag_var} MATCHES "-D_DEBUG")
    ENDFOREACH(flag_var)
ENDIF (USE_MSVC_RELEASE_RUNTIME_ALWAYS)

add_compile_definitions(EDITOR_MODE=1)
add_subdirectory(libs/igeScene ${CMAKE_BINARY_DIR}/igeScene)
add_subdirectory(libs/pyxCore/pyxcore ${CMAKE_BINARY_DIR}/pyxCore)

# Source files
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/pfd/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/pfd/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/ImGuizmo/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/ImGuizmo/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/*.cpp"
)

set (EXCLUDE_DIR
    "/examples"
    "/example"
    "/bin"
    "/misc"
    "/docs"
)
foreach (TMP_PATH ${SRC_FILES})
    foreach (TMP_DIR ${EXCLUDE_DIR})
        string (FIND ${TMP_PATH} ${TMP_DIR} EXCLUDE_DIR_FOUND)
        if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
            list (REMOVE_ITEM SRC_FILES ${TMP_PATH})
        endif ()
    endforeach(TMP_DIR)
endforeach(TMP_PATH)

set(SRC_FILES ${SRC_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/examples/imgui_impl_opengl3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/examples/imgui_impl_sdl.cpp"
)

add_executable(${TARGET_NAME} WIN32 ${SRC_FILES})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099 /ignore:4286 /ignore:4127") # Disable linker warning
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT") # Disable LIBCMT, avoid conflict definition

target_link_libraries(${TARGET_NAME} 
    igeScene-static
    pyxCore-static
    ${CONAN_LIBS}
    Crypt32
    opengl32
    glu32
    imagehlp
    dinput8
    dxguid
    user32
    gdi32
    winmm
    imm32
    ole32
    oleaut32
    shell32
    version
    uuid
    Setupapi
    shlwapi
    ws2_32
    Psapi
    Kernel32
    doboz edgeanim simplecpp tokenizer GLEW
)

target_include_directories(${TARGET_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/pfd"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/ImGuizmo"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/igeScene"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/pyxcore"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/GLEW"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/edge/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/vectormath/scalar/cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/doboz"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/simplecpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/tokenizer"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/spdlog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/spdlog/include/spdlog"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/igeVMath/"
    "${CONAN_INCLUDE_DIRS}"
)

target_compile_definitions(${TARGET_NAME} PUBLIC ${CONAN_DEFINES} Py_NO_ENABLE_SHARED)

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/app")

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/app/$<0:>")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
