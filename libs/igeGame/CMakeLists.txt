cmake_minimum_required(VERSION 3.4.1)

set(TARGET_NAME igeGame)

project(${TARGET_NAME})

set (CMAKE_CXX_STANDARD 17)

if(DEFINED ANDROID_PLATFORM)
    set (ANDROID 1)
    if(ANDROID_ABI STREQUAL "armeabi-v7a")
        add_compile_options(-marm -march=armv7-a -mtune=cortex-a15 -mfpu=neon-fp16 -mfloat-abi=softfp)
    elseif(ANDROID_ABI STREQUAL "x86")
        add_compile_options(-march=atom -m32 -msse -msse2 -msse3 -mssse3 -mfpmath=sse -mstackrealign)
    elseif(ANDROID_ABI STREQUAL "arm64-v8a")
        add_compile_options(-march=armv8-a+simd+sve -mtune=cortex-a53 -D__ARM_NEON -D__ARM_NEON__)
    elseif(ANDROID_ABI STREQUAL "x86_64")
        add_compile_options(-march=atom -m64 -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mfpmath=sse)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-g0 -Ofast)
    else(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-Og)
    endif()
elseif(MSVC)
    if(${APP_STYLE} MATCHES "SHARED")
        set(CMAKE_CXX_FLAGS "/MD ${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /Zc:inline")
        set(CMAKE_CXX_FLAGS_DEBUG "/MD /JMC /ZI /Od")
    endif()
    add_compile_options(/MP)
elseif(APPLE)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-g0 -Ofast)
    endif()
endif()

set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

set (PYTHON_INCLUDE_DIRS ${LIBS_DIR}/Python/Include)
if (MSVC)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/Python/PC)
elseif (OSX)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/Python/Mac)
elseif (ANDROID)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/Python/Android)
elseif (IOS)
    list(APPEND PYTHON_INCLUDE_DIRS ${LIBS_DIR}/Python/IOS)
endif ()

file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(${TARGET_NAME} WIN32 ${SRC_FILES})

if(NOT DEFINED BUILD_PLATFORM)
    set (BUILD_PLATFORM pc)
endif()

if(NOT DEFINED BUILD_ARCH)
    set (BUILD_ARCH x64)
endif()

if(MSVC)
    set (PLATFORM_PATH libs/${BUILD_PLATFORM}/${BUILD_ARCH}/)
    set (PLATFORM_PATH_DEBUG libs/${BUILD_PLATFORM}/Debug/${BUILD_ARCH}/)
    set (LIB_EXT lib)
else()
    set (PLATFORM_PATH libs/${BUILD_PLATFORM}/${BUILD_ARCH}/lib)
    set (LIB_EXT a)
endif()

# Python
set (LIB_PATH ${LIBS_DIR}/Python/${PLATFORM_PATH})
target_link_libraries(${TARGET_NAME}
    ${LIB_PATH}Python37.${LIB_EXT}
)

# igeCore
set (LIB_PATH ${LIBS_DIR}/pyxCore/${PLATFORM_PATH})
set (LIB_PATH_DEBUG ${LIBS_DIR}/pyxCore/${PLATFORM_PATH_DEBUG})
target_link_libraries(${TARGET_NAME}
    optimized ${LIB_PATH}pyxcore-static.${LIB_EXT}
    optimized ${LIB_PATH}edgeanim.${LIB_EXT}
    optimized ${LIB_PATH}doboz.${LIB_EXT}
    optimized ${LIB_PATH}tokenizer.${LIB_EXT}
    optimized ${LIB_PATH}simplecpp.${LIB_EXT}
    debug ${LIB_PATH_DEBUG}pyxcore-static.${LIB_EXT}
    debug ${LIB_PATH_DEBUG}edgeanim.${LIB_EXT}
    debug ${LIB_PATH_DEBUG}doboz.${LIB_EXT}
    debug ${LIB_PATH_DEBUG}tokenizer.${LIB_EXT}
    debug ${LIB_PATH_DEBUG}simplecpp.${LIB_EXT}
)
if(MSVC)
    target_link_libraries(${TARGET_NAME}
        optimized ${LIB_PATH}pyxtools-static.${LIB_EXT}
        optimized ${LIB_PATH}GLEW.${LIB_EXT}
        debug ${LIB_PATH_DEBUG}pyxtools-static.${LIB_EXT}
        debug ${LIB_PATH_DEBUG}GLEW.${LIB_EXT}
    )
endif()

set (LIB_PATH ${LIBS_DIR}/igeCore/${PLATFORM_PATH})
set (LIB_PATH_DEBUG ${LIBS_DIR}/igeCore/${PLATFORM_PATH_DEBUG})
target_link_libraries(${TARGET_NAME}
    optimized ${LIB_PATH}igeCore-static.${LIB_EXT}
    debug ${LIB_PATH_DEBUG}igeCore-static.${LIB_EXT}
)

# igeVmath
set (LIB_PATH ${LIBS_DIR}/igeVMath/${PLATFORM_PATH})
target_link_libraries(${TARGET_NAME} ${LIB_PATH}igeVMath-static.${LIB_EXT})
    
# igeBullet
set (LIB_PATH ${LIBS_DIR}/igeBullet/${PLATFORM_PATH})
set (LIB_PATH_DEBUG ${LIBS_DIR}/igeBullet/${PLATFORM_PATH_DEBUG})
target_link_libraries(${TARGET_NAME}
    optimized ${LIB_PATH}igeBullet-static.${LIB_EXT}
    debug ${LIB_PATH_DEBUG}igeBullet-static.${LIB_EXT}
)

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
    ${PYTHON_INCLUDE_DIRS}
    ${LIBS_DIR}/pyxCore/include/pyxcore
    ${LIBS_DIR}/pyxCore/include/ThirdParty/GLEW
    ${LIBS_DIR}/pyxCore/include/ThirdParty/edge/include
    ${LIBS_DIR}/pyxCore/include/ThirdParty/spdlog/include
    ${LIBS_DIR}/tracy/include
    ${LIBS_DIR}/json/include
    ${LIBS_DIR}/igeVmath/include
    ${LIBS_DIR}/igeCore/include
    ${LIBS_DIR}/igeScene
    ${LIBS_DIR}/SDL/include
    ${LIBS_DIR}/stb/include
    ${LIBS_DIR}/igeBullet/include
    ${LIBS_DIR}/igeBullet/include/bullet
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