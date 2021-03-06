cmake_minimum_required(VERSION 3.4.1)

set(TARGET_NAME igeCreator)

project(${TARGET_NAME})

set(BUILD_FULL_SOURCE FALSE)
set(USE_MSVC_RELEASE_RUNTIME_ALWAYS TRUE)

if (BUILD_FULL_SOURCE)
    file(TO_CMAKE_PATH $ENV{IGE_LIBS} IGE_LIBS)
    set(LIBS_DIR ${IGE_LIBS})
else()
    set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/igeLibs)
endif()

IF (USE_MSVC_RELEASE_RUNTIME_ALWAYS)
    FOREACH(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
        IF(${flag_var} MATCHES "/MDd")
            STRING(REGEX REPLACE "/MDd" "/MD" ${flag_var} "${${flag_var}}")
        ENDIF(${flag_var} MATCHES "/MDd")
        IF(${flag_var} MATCHES "/MTd")
            STRING(REGEX REPLACE "/MTd" "/MT" ${flag_var} "${${flag_var}}")
        ENDIF(${flag_var} MATCHES "/MTd")
        # Need to remove _DEBUG too otherwise things like _ITERATOR_DEBUG_LEVEL mismatch
        IF(${flag_var} MATCHES "-D_DEBUG")
            STRING(REGEX REPLACE "-D_DEBUG" "" ${flag_var} "${${flag_var}}")
        ENDIF(${flag_var} MATCHES "-D_DEBUG")
    ENDFOREACH(flag_var)
ENDIF (USE_MSVC_RELEASE_RUNTIME_ALWAYS)

if(NOT DEFINED BUILD_PLATFORM)
    set (BUILD_PLATFORM pc)
endif()

if(NOT DEFINED BUILD_ARCH)
    set (BUILD_ARCH x64)
endif()

if(MSVC)
    set (PLATFORM_PATH libs/${BUILD_PLATFORM}/${BUILD_ARCH}/)
    set (LIB_EXT lib)
else()
    set (PLATFORM_PATH libs/${BUILD_PLATFORM}/${BUILD_ARCH}/lib)
    set (LIB_EXT a)
endif()

set(CMAKE_CXX_STANDARD 17)
set(APP_STYLE STATIC)

add_compile_definitions(EDITOR_MODE=1)
add_subdirectory(libs/igeScene  ${CMAKE_BINARY_DIR}/igeScene)

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

# Python
set (LIB_PATH ${LIBS_DIR}/Python/${PLATFORM_PATH})
target_link_libraries(${TARGET_NAME}
    ${LIB_PATH}Python37.${LIB_EXT}
)

# igeCore
if (BUILD_FULL_SOURCE)
    add_subdirectory(${IGE_LIBS}/../pyxCore/pyxcore ${CMAKE_BINARY_DIR}/pyxcore-static)
    target_link_libraries(${TARGET_NAME} pyxcore-static)

    add_subdirectory(${IGE_LIBS}/../pyxCore/pyxtools ${CMAKE_BINARY_DIR}/pyxtools-static)
    target_link_libraries(${TARGET_NAME} pyxtools-static)

    add_subdirectory(${IGE_LIBS}/../igeCore ${CMAKE_BINARY_DIR}/igeCore-static)
    target_link_libraries(${TARGET_NAME} igeCore-static)
else()
    set (LIB_PATH ${LIBS_DIR}/pyxCore/${PLATFORM_PATH})
    target_link_libraries(${TARGET_NAME}
        ${LIB_PATH}pyxcore-static.${LIB_EXT}
        ${LIB_PATH}edgeanim.${LIB_EXT}
        ${LIB_PATH}doboz.${LIB_EXT}
        ${LIB_PATH}tokenizer.${LIB_EXT}
        ${LIB_PATH}simplecpp.${LIB_EXT}
    )
    if(MSVC)
        target_link_libraries(${TARGET_NAME}
            ${LIB_PATH}pyxtools-static.${LIB_EXT}
            ${LIB_PATH}GLEW.${LIB_EXT}
        )
    endif()

    set (LIB_PATH ${LIBS_DIR}/igeCore/${PLATFORM_PATH})
    target_link_libraries(${TARGET_NAME}
        ${LIB_PATH}igeCore-static.${LIB_EXT}
    )
endif()

# igeVmath
set (LIB_PATH ${LIBS_DIR}/igeVMath/${PLATFORM_PATH})
target_link_libraries(${TARGET_NAME} ${LIB_PATH}igeVMath-static.${LIB_EXT})

# igeBullet
if (BUILD_FULL_SOURCE)
    add_subdirectory(${IGE_LIBS}/../igeBullet ${CMAKE_BINARY_DIR}/igeBullet-static)
    target_link_libraries(${TARGET_NAME} igeBullet-static)
else()
    set (LIB_PATH ${LIBS_DIR}/igeBullet/${PLATFORM_PATH})
    target_link_libraries(${TARGET_NAME}
        ${LIB_PATH}igeBullet-static.${LIB_EXT}
    )    
endif()

# bullet
set (LIB_PATH ${LIBS_DIR}/bullet/${PLATFORM_PATH})
target_link_libraries(${TARGET_NAME} 
    ${LIB_PATH}Bullet3Common.${LIB_EXT}
    ${LIB_PATH}BulletCollision.${LIB_EXT}
    ${LIB_PATH}BulletDynamics.${LIB_EXT}
    ${LIB_PATH}BulletFileLoader.${LIB_EXT}
    ${LIB_PATH}BulletInverseDynamics.${LIB_EXT}
    ${LIB_PATH}BulletInverseDynamicsUtils.${LIB_EXT}
    ${LIB_PATH}BulletRobotics.${LIB_EXT}
    ${LIB_PATH}BulletRoboticsGUI.${LIB_EXT}
    ${LIB_PATH}BulletSoftBody.${LIB_EXT}
    ${LIB_PATH}BulletWorldImporter.${LIB_EXT}
    ${LIB_PATH}BulletXmlWorldImporter.${LIB_EXT}
    ${LIB_PATH}ConvexDecomposition.${LIB_EXT}
    ${LIB_PATH}GIMPACTUtils.${LIB_EXT}
    ${LIB_PATH}HACD.${LIB_EXT}
    ${LIB_PATH}LinearMath.${LIB_EXT}
)

# igeSound
set (LIB_PATH ${LIBS_DIR}/igeSound/${PLATFORM_PATH})
target_link_libraries(${TARGET_NAME}
    ${LIB_PATH}igeSound-static.${LIB_EXT}
)

# igeEffekseer
set (LIB_PATH ${LIBS_DIR}/igeEffekseer/${PLATFORM_PATH})
target_link_libraries(${TARGET_NAME}
    ${LIB_PATH}igeEffekseer-static.${LIB_EXT}
    ${LIB_PATH}Effekseer.${LIB_EXT}
    ${LIB_PATH}EffekseerRendererGL.${LIB_EXT}
)

# igeNavigation
if (BUILD_FULL_SOURCE)
    add_subdirectory(${IGE_LIBS}/../igeNavigation ${CMAKE_BINARY_DIR}/igeNavigation-static)
    target_link_libraries(${TARGET_NAME} igeNavigation-static)
else()
    set (LIB_PATH ${LIBS_DIR}/igeNavigation/${PLATFORM_PATH})
    target_link_libraries(${TARGET_NAME}
        ${LIB_PATH}igeNavigation-static.${LIB_EXT}
        ${LIB_PATH}Recast.${LIB_EXT}
        ${LIB_PATH}Detour.${LIB_EXT}
        ${LIB_PATH}DetourCrowd.${LIB_EXT}
        ${LIB_PATH}DetourTileCache.${LIB_EXT}
    )
endif()

# SDL
set (LIB_PATH ${LIBS_DIR}/SDL/${PLATFORM_PATH})
target_link_libraries(${TARGET_NAME} ${LIB_PATH}SDL2.${LIB_EXT})
if(ANDROID)
    target_link_libraries(${TARGET_NAME} ${LIB_PATH}hidapi.${LIB_EXT})
endif()

# Other pre-built libs
set (LIB_PATH ${LIBS_DIR}/openssl/${PLATFORM_PATH})
target_link_libraries(${TARGET_NAME}
    ${LIB_PATH}ssl.${LIB_EXT}
    ${LIB_PATH}crypto.${LIB_EXT}
)

set (LIB_PATH ${LIBS_DIR}/stb/${PLATFORM_PATH})
target_link_libraries(${TARGET_NAME}
    ${LIB_PATH}stb_image.${LIB_EXT}
    ${LIB_PATH}stb_image_write.${LIB_EXT}
    ${LIB_PATH}stb_truetype.${LIB_EXT}
)

# ZLIB (only Windows)
if(MSVC)
    set (LIB_PATH ${LIBS_DIR}/zlib/${PLATFORM_PATH})
    target_link_libraries(${TARGET_NAME}
        ${LIB_PATH}zlib.${LIB_EXT}
    )
endif()

set (PYTHON_INCLUDE_DIRS "${LIBS_DIR}/Python/Include")
if (MSVC)
    list(APPEND PYTHON_INCLUDE_DIRS "${LIBS_DIR}/Python/PC")
elseif (OSX)
    list(APPEND PYTHON_INCLUDE_DIRS "${LIBS_DIR}/Python/Mac")
elseif (ANDROID)
    list(APPEND PYTHON_INCLUDE_DIRS "${LIBS_DIR}/Python/Android")
elseif (IOS)
    list(APPEND PYTHON_INCLUDE_DIRS "${LIBS_DIR}/Python/IOS")
endif ()

target_link_libraries(${TARGET_NAME}
    igeScene-static
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
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/pfd
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/ImGuizmo
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/igeScene
    ${PYTHON_INCLUDE_DIRS}
    ${LIBS_DIR}/pyxCore/include/pyxcore
    ${LIBS_DIR}/pyxCore/include/ThirdParty/GLEW
    ${LIBS_DIR}/pyxCore/include/ThirdParty/edge/include
    ${LIBS_DIR}/pyxCore/include/ThirdParty/spdlog/include
    ${LIBS_DIR}/tracy/include
    ${LIBS_DIR}/json/include
    ${LIBS_DIR}/igeVmath/include
    ${LIBS_DIR}/igeCore/include
    ${LIBS_DIR}/SDL/include
    ${LIBS_DIR}/stb/include
    ${LIBS_DIR}/bullet/include
    ${LIBS_DIR}/bullet/include/src
    ${LIBS_DIR}/igeBullet/include
    ${LIBS_DIR}/igeBullet/include/bullet
    ${LIBS_DIR}/igeSound/include
    ${LIBS_DIR}/igeSound/include/ThirdParty/soloud/include
    ${LIBS_DIR}/igeEffekseer/include/ThirdParty/Effekseer/Dev/Cpp
    ${LIBS_DIR}/igeEffekseer/include/ThirdParty/Effekseer/Dev/Cpp/Effekseer
    ${LIBS_DIR}/igeEffekseer/include/ThirdParty/Effekseer/Dev/Cpp/EffekseerRendererCommon
    ${LIBS_DIR}/igeNavigation/include
    ${LIBS_DIR}/igeNavigation/include/3rd-party/recast/Recast/Include
    ${LIBS_DIR}/igeNavigation/include/3rd-party/recast/Detour/Include
    ${LIBS_DIR}/igeNavigation/include/3rd-party/recast/DetourCrowd/Include
    ${LIBS_DIR}/igeNavigation/include/3rd-party/recast/DetourTileCache/Include
)

# Python37
set (DEFINITIONS Py_NO_ENABLE_SHARED)
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(DEFINITIONS ${DEFINITIONS} _AMD64_)
else()
    set(DEFINITIONS ${DEFINITIONS} _X86_)
endif()
target_compile_definitions(${TARGET_NAME} PUBLIC ${DEFINITIONS})

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/app"
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
