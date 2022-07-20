cmake_minimum_required(VERSION 3.10.2)

set(TARGET_NAME #TARGET_NAME#)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 17)
add_compile_options(/MP /std:c++17)

if(#DEBUG_BUILD#)
    option(USE_PREBUILT_LIBS "Use prebuilt libraries" OFF)
else()
    option(USE_PREBUILT_LIBS "Use prebuilt libraries" ON)
endif()

#CMAKE_DEPENDENCIES#

set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(USE_MSVC_RELEASE_RUNTIME_ALWAYS ON)
if(USE_MSVC_RELEASE_RUNTIME_ALWAYS)
    foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
        if(${flag_var} MATCHES "/MDd")
            STRING(REGEX REPLACE "/MDd" "/MD" ${flag_var} "${${flag_var}}")
        endif(${flag_var} MATCHES "/MDd")
        if(${flag_var} MATCHES "/MTd")
            STRING(REGEX REPLACE "/MTd" "/MT" ${flag_var} "${${flag_var}}")
        endif(${flag_var} MATCHES "/MTd")
        if(${flag_var} MATCHES "-D_DEBUG")
            STRING(REGEX REPLACE "-D_DEBUG" "" ${flag_var} "${${flag_var}}")
        endif(${flag_var} MATCHES "-D_DEBUG")
    endforeach()
endif()

set(SRC_FILES
    "${PROJECT_DIR}/src/Game.h"
    "${PROJECT_DIR}/src/Game.cpp"
    "${PROJECT_DIR}/src/pythonExtension.h"
    "${PROJECT_DIR}/src/pythonExtension.c"
    "${PROJECT_DIR}/src/pythonStarter.h"
    "${PROJECT_DIR}/src/pythonStarter.cpp"
    "${PROJECT_DIR}/windows/main.cpp"
)

add_executable(${TARGET_NAME} WIN32 ${SRC_FILES})
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "#APP_NAME#")

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

target_compile_definitions(${TARGET_NAME} PUBLIC ${COMPILE_DEFINITIONS} USE_NUMPY)

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
    Iphlpapi
    Pathcch
)

target_compile_definitions(${TARGET_NAME} PUBLIC Py_NO_ENABLE_SHARED)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/app"
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099 /ignore:4286 /ignore:4127") # Disable linker warning
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libvcruntime /NODEFAULTLIB:libvcruntimed")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:python37 /NODEFAULTLIB:python39")
