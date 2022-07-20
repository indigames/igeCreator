cmake_minimum_required(VERSION 3.10.2)

set(TARGET_NAME #TARGET_NAME#)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 17)

if(#DEBUG_BUILD#)
    option(USE_PREBUILT_LIBS "Use prebuilt libraries" OFF)
else()
    option(USE_PREBUILT_LIBS "Use prebuilt libraries" ON)
endif()

#CMAKE_DEPENDENCIES#

set(BUILD_PLATFORM android)
set(BUILD_ARCH ${ANDROID_ABI})

option(USE_PREBUILT_LIBS "Use prebuilt libraries" OFF)

set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

if(NOT DEFINED ANDROID)
    set(ANDROID 1)
    add_compile_options(-D__ANDROID__ -DANDROID -Wunknown-warning-option)
endif()

add_compile_options(
    "$<$<CONFIG:DEBUG>:-Og>"
    "$<$<CONFIG:RELEASE>:-Ofast>"
)

set(SRC_FILES
    "${PROJECT_DIR}/src/Game.h"
    "${PROJECT_DIR}/src/Game.cpp"
    "${PROJECT_DIR}/src/pythonExtension.h"
    "${PROJECT_DIR}/src/pythonExtension.c"
    "${PROJECT_DIR}/src/pythonStarter.h"
    "${PROJECT_DIR}/src/pythonStarter.cpp"
    "${PROJECT_DIR}/android/app/src/main/cpp/native-lib.cpp"
    "${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c"
)

add_library(${TARGET_NAME} SHARED ${SRC_FILES})

target_link_libraries(${TARGET_NAME} -Wl,--start-group)

set (CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}
    ${PROJECT_DIR}/../../../cmake
    ${PROJECT_DIR}/../../../libs/igeScene/cmake
    ${PROJECT_DIR}/../../../libs/igeScene/libs/igeCore/cmake
    ${PROJECT_DIR}/../../../libs/igeScene/libs/igeCore/libs/pyxCore/cmake
    ${PROJECT_DIR}/../../../libs/igeScene/libs/igeCore/libs/pyxCore/pyxtools/cmake
    ${PROJECT_DIR}/../../../libs/igeScene/libs/igeBullet/cmake
    ${PROJECT_DIR}/../../../libs/igeScene/libs/igeEffekseer/cmake
    ${PROJECT_DIR}/../../../libs/igeScene/libs/igeNavigation/cmake
    ${PROJECT_DIR}/../../../libs/igeScene/libs/igeSound/cmake
    ${PROJECT_DIR}/../../../libs/igeSdk/cmake
    ${CMAKE_MODULE_PATH}
)

include(FindPython)
include(Findnumpy)
include(Findopenssl)
include(FindigeVmath)
include(FindSDL)
include(Findstb)
include(FindpyxCore)
include(FindigeCore)
if(USE_IGESCENE)
    include(Findtaskflow)
    include(Findjson)
    include(FindigeScene)
    include(FindigeSound)
    include(FindigeEffekseer)
    include(FindigeNavigation)
endif()
if(USE_IGEBULLET)
    include(Findbullet)
    include(FindigeBullet)
endif()
if(USE_IGEEFFEKSEER)
    include(FindigeEffekseer)
endif()
if(USE_IGESOUND)
    include(FindigeSound)
endif()
if(USE_IGESDK)
    include(FindigeSdk)
endif()

include(Findzlib)

target_include_directories(${TARGET_NAME} PUBLIC
    "${PROJECT_DIR}/src"
    "${pyxCore_INCLUDE_DIRS}"
    "${igeCore_INCLUDE_DIRS}"
    "${igeVmath_INCLUDE_DIRS}"
    "${Python_INCLUDE_DIRS}"
    "${numpy_INCLUDE_DIRS}"
    "${bullet_INCLUDE_DIRS}"
    "${igeBullet_INCLUDE_DIRS}"
    "${igeVmath_INCLUDE_DIRS}"
    "${taskflow_INCLUDE_DIRS}"
    "${json_INCLUDE_DIRS}"
    "${SDL_INCLUDE_DIRS}"
    "${stb_INCLUDE_DIRS}"
    "${json_INCLUDE_DIRS}"
    "${igeSound_INCLUDE_DIRS}"
    "${igeEffekseer_INCLUDE_DIRS}"
    "${igeNavigation_INCLUDE_DIRS}"
    "${igeScene_INCLUDE_DIRS}"
)

set(COMPILE_DEFINITIONS
    ${pyxCore_COMPILE_DEFINITIONS}
    ${igeCore_COMPILE_DEFINITIONS}
    ${igeScene_COMPILE_DEFINITIONS}
    ${igeBullet_COMPILE_DEFINITIONS}
    ${igeEffekseer_COMPILE_DEFINITIONS}
    ${igeNavigation_COMPILE_DEFINITIONS}
    ${igeSound_COMPILE_DEFINITIONS}
    ${igeVmath_COMPILE_DEFINITIONS}
    ${igeSdk_COMPILE_DEFINITIONS}
)

message(COMPILE_DEFINITIONS = ${COMPILE_DEFINITIONS})

target_compile_definitions(${TARGET_NAME} PUBLIC ${COMPILE_DEFINITIONS} USE_NUMPY Py_NO_ENABLE_SHARED)

target_link_directories(${TARGET_NAME} PUBLIC
    ${Python_LIB_DIRS}
    ${numpy_LIB_DIRS}
    ${igeVmath_LIB_DIRS}
    ${bullet_LIB_DIRS}
    ${igeBullet_LIB_DIRS}
    ${stb_LIB_DIRS}
    ${SDL_LIB_DIRS}
    ${igePAL_LIB_DIRS}
    ${igeSdk_LIB_DIRS}
    ${igeScene_LIB_DIRS}
    ${igeSound_LIB_DIRS}
    ${igeEffekseer_LIB_DIRS}
    ${igeNavigation_LIB_DIRS}
    ${igeCore_LIB_DIRS}
    ${pyxCore_LIB_DIRS}
    ${openssl_LIB_DIRS}
    ${zlib_LIB_DIRS}
)

target_link_libraries(${TARGET_NAME}
    ${Python_LIBRARIES}
    ${openssl_LIBRARIES}
    ${numpy_LIBRARIES}
    ${tracy_LIBRARIES}
    ${bullet_LIBRARIES}
    ${igeBullet_LIBRARIES}
    ${stb_LIBRARIES}
    ${SDL_LIBRARIES}
    ${igeSound_LIBRARIES}
    ${igeEffekseer_LIBRARIES}
    ${igeNavigation_LIBRARIES}
    ${igeVmath_LIBRARIES}
    ${igeCore_LIBRARIES}
    ${pyxCore_LIBRARIES}
    ${igeScene_LIBRARIES}
    ${igeSdk_LIBRARIES}
    ${zlib_LIBRARIES}
)

target_link_libraries(${TARGET_NAME}
        android
    EGL
    GLESv1_CM
    GLESv2
    GLESv3
    OpenSLES
    log
    z
)
target_link_libraries(${TARGET_NAME} -Wl,--end-group)