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

set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(SRC_FILES
    "${PROJECT_DIR}/src/Game.h"
    "${PROJECT_DIR}/src/Game.cpp"
    "${PROJECT_DIR}/src/pythonExtension.h"
    "${PROJECT_DIR}/src/pythonExtension.c"
    "${PROJECT_DIR}/src/pythonStarter.h"
    "${PROJECT_DIR}/src/pythonStarter.cpp"
    "${PROJECT_DIR}/emscripten/main.cpp"
)

add_executable(${TARGET_NAME} ${SRC_FILES})
set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-m32 -s WASM=1 -s FORCE_FILESYSTEM=1 -pthread -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=1 -s TOTAL_MEMORY=512MB -s OFFSCREENCANVAS_SUPPORT=1 -s MODULARIZE=1 -s FETCH=1 -s CASE_INSENSITIVE_FS=1 -s EXPORT_NAME='MyModule' -gsource-map -s DEMANGLE_SUPPORT=1 --bind")

set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "-m32 -pthread -s USE_PTHREADS=1")

list (APPEND CMAKE_MODULE_PATH
    "${PROJECT_DIR}/../../../libs/igeSdk/cmake"
    "${PROJECT_DIR}/../../../libs/igeScene/cmake"
    "${PROJECT_DIR}/../../../libs/igeScene/libs/igeSound/cmake"
    "${PROJECT_DIR}/../../../libs/igeScene/libs/igeBullet/cmake"
    "${PROJECT_DIR}/../../../libs/igeScene/libs/igeEffekseer/cmake"
    "${PROJECT_DIR}/../../../libs/igeScene/libs/igeNavigation/cmake"
    "${PROJECT_DIR}/../../../libs/igeScene/libs/igeCore/cmake"
    "${PROJECT_DIR}/../../../libs/igeScene/libs/igeCore/libs/pyxCore/cmake"
    "${PROJECT_DIR}/../../../libs/igeScene/libs/igeCore/libs/pyxCore/pyxtools/cmake"
    "${PROJECT_DIR}/../../../libs/igeScene/libs/igeCore/libs/pyxCore/libs/igeVmath/cmake"
    "${PROJECT_DIR}/../../../cmake"
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

message(Python = ${Python_LIB_DIRS})

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
)
