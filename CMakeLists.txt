cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0091 NEW)

set(TARGET_NAME igeCreator)

project(${TARGET_NAME})

set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(CMAKE_PROJECT_VERSION_PATCH 6)
set(CMAKE_PROJECT_VERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH})

# Modules and install path (required by conan)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

# Set the Relative Path Configurations
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

#####################################
# CMake Module Imports
#####################################
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(InstallRequiredSystemLibraries)
include(GenerateExportHeader)

#####################################
# CPack Configurations
#####################################
set(CPACK_PACKAGE_NAME "${TARGET_NAME}")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${TARGET_NAME}")
set(CPACK_PACKAGE_VENDOR "Indi Games")
set(CPACK_PACKAGE_VERSION_MAJOR "${CMAKE_PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CMAKE_PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CMAKE_PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_GENERATOR "ZIP;NSIS")
set(CPACK_STRIP_FILES "TRUE")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_LICENSE_PROVIDED "1")
set(CPACK_NSIS_CONTACT "admin@indigames.net")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/igeCreator.ico")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_CURRENT_SOURCE_DIR}/src/igeCreator.ico")
set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${TARGET_NAME}.lnk' '$INSTDIR\\\\bin\\\\${TARGET_NAME}.exe'")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "CreateShortCut \\\"$DESKTOP\\\\${TARGET_NAME}.lnk\\\" \\\"$INSTDIR\\\\bin\\\\${TARGET_NAME}.exe\\\"")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "Delete \\\"$DESKTOP\\\\${TARGET_NAME}.lnk\\\"")
set(CPACK_NSIS_MENU_LINKS "doc" "documents")
include(CPack)

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
    add_compile_options(/MP /std:c++17)
elseif(IOS)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-g0 -Ofast)
    endif()
endif()

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/pyxcore"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/pyxtools"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/GLEW"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/edge/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/vectormath/scalar/cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/doboz"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/simplecpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/tokenizer"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/spdlog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/pyxCore/ThirdParty/spdlog/include/spdlog"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/igeCore"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/igeVMath"
)

add_compile_definitions(EDITOR_MODE=1 IMGUI_DEFINE_MATH_OPERATORS=1)
add_subdirectory(libs/pyxCore ${CMAKE_BINARY_DIR}/pyxCore EXCLUDE_FROM_ALL)
add_subdirectory(libs/igeCore ${CMAKE_BINARY_DIR}/igeCore EXCLUDE_FROM_ALL)
add_subdirectory(libs/igeVMath ${CMAKE_BINARY_DIR}/igeVMath EXCLUDE_FROM_ALL)
add_subdirectory(libs/igeScene ${CMAKE_BINARY_DIR}/igeScene EXCLUDE_FROM_ALL)

if(${APP_STYLE} MATCHES "SHARED")
    find_package (Python3 ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Development)
else()
    find_package(Python REQUIRED)
    set (Python3_INCLUDE_DIRS ${Python_INCLUDE_DIRS})
    set (Python3_LIBRARIES ${Python_LIBRARIES})
endif()

find_package(numpy REQUIRED)
find_package(stb REQUIRED)
find_package(SDL REQUIRED)
find_package(openssl REQUIRED)
find_package(taskflow REQUIRED)
find_package(tracy REQUIRED)
find_package(zlib)
find_package(json)
find_package(igeBullet REQUIRED)
find_package(bullet REQUIRED)
find_package(igeSound REQUIRED)
find_package(igeEffekseer REQUIRED)
find_package(igeNavigation REQUIRED)
find_package(igeSdk REQUIRED)

include_directories(
    "${zlib_INCLUDE_DIRS}"
    "${json_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "${numpy_INCLUDE_DIRS}"
    "${stb_INCLUDE_DIRS}"
    "${SDL_INCLUDE_DIRS}"
    "${openssl_INCLUDE_DIRS}"
    "${taskflow_INCLUDE_DIRS}"
    "${tracy_INCLUDE_DIRS}"
    "${igeBullet_INCLUDE_DIRS}"
    "${bullet_INCLUDE_DIRS}"
    "${igeSound_INCLUDE_DIRS}"
    "${igeSound_INCLUDE_DIRS}/ThirdParty/soloud/include"
    "${igeEffekseer_INCLUDE_DIRS}"
    "${igeEffekseer_INCLUDE_DIRS}/ThirdParty/Effekseer/Dev/Cpp"
    "${igeEffekseer_INCLUDE_DIRS}/ThirdParty/Effekseer/Dev/Cpp/Effekseer"
    "${igeNavigation_INCLUDE_DIRS}"
    "${igeNavigation_INCLUDE_DIRS}/3rd-party/recast/Detour/Include"
    "${igeNavigation_INCLUDE_DIRS}/3rd-party/recast/DetourCrowd/Include"
    "${igeNavigation_INCLUDE_DIRS}/3rd-party/recast/DetourTileCache/Include"
    "${igeNavigation_INCLUDE_DIRS}/3rd-party/recast/Recast/Include"
    "${igeSdk_INCLUDE_DIRS}"
)

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
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui-node-editor/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui-node-editor/*.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui-node-editor/*.cpp"
)

set (EXCLUDE_DIR
    "/examples"
    "/example"
    "/external"
    "/bin"
    "/build"
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
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/imgui_tables.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/imgui_stacklayout.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/backends/imgui_impl_opengl3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui/backends/imgui_impl_sdl.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})
add_executable(${TARGET_NAME} WIN32 "${CMAKE_CURRENT_SOURCE_DIR}/src/igeCreator.rc" ${SRC_FILES})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099 /ignore:4286 /ignore:4127") # Disable linker warning
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT") # Disable LIBCMT, avoid conflict definition
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:python37") # Disable python37, avoid conflict definition

message(STATUS "Python3_LIBRARIES: ${Python3_LIBRARIES}")
message(STATUS "numpy_LIBRARIES: ${numpy_LIBRARIES}")
message(STATUS "tracy_LIBRARIES: ${tracy_LIBRARIES}")
message(STATUS "bullet_LIBRARIES: ${bullet_LIBRARIES}")
message(STATUS "igeBullet_LIBRARIES: ${igeBullet_LIBRARIES}")
message(STATUS "stb_LIBRARIES: ${stb_LIBRARIES}")
message(STATUS "SDL_LIBRARIES: ${SDL_LIBRARIES}")
message(STATUS "openssl_LIBRARIES: ${openssl_LIBRARIES}")
message(STATUS "igeSound_LIBRARIES: ${igeSound_LIBRARIES}")
message(STATUS "igeEffekseer_LIBRARIES: ${igeEffekseer_LIBRARIES}")
message(STATUS "igeNavigation_LIBRARIES: ${igeNavigation_LIBRARIES}")

set (LIBS_TO_REMOVE "igeVmath::igeVmath" "pyxCore::pyxCore" )
foreach(elem ${LIBS_TO_REMOVE})
    list(REMOVE_ITEM igeBullet_LIBRARIES ${elem})
    list(REMOVE_ITEM igeEffekseer_LIBRARIES ${elem})
endforeach()

target_link_libraries(${TARGET_NAME} 
    igeScene-static
    pyxcore
    pyxtools
    igeCore-static
    igeVmath-static
    ${Python3_LIBRARIES}
    ${numpy_LIBRARIES}
    ${tracy_LIBRARIES}
    ${bullet_LIBRARIES}
    ${igeBullet_LIBRARIES}
    ${stb_LIBRARIES}
    ${SDL_LIBRARIES}
    ${openssl_LIBRARIES}
    ${igeSound_LIBRARIES}
    ${igeEffekseer_LIBRARIES}
    ${igeNavigation_LIBRARIES}
    ${igeSdk_LIBRARIES}
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
    doboz
    edgeanim
    simplecpp
    tokenizer
    GLEW
    Iphlpapi
    Pathcch
)

target_include_directories(${TARGET_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/pfd"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/ImGuizmo"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui-node-editor"
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
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/igeCore"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/igeVMath"
)

set(COMPILE_DEFINITIONS
    ${Python3_COMPILE_DEFINITIONS}
    ${numpy_COMPILE_DEFINITIONS}
    ${tracy_COMPILE_DEFINITIONS}
    ${bullet_COMPILE_DEFINITIONS}
    ${igeBullet_COMPILE_DEFINITIONS}
    ${stb_COMPILE_DEFINITIONS}
    ${SDL_COMPILE_DEFINITIONS}
    ${openssl_COMPILE_DEFINITIONS}
    ${igeSound_COMPILE_DEFINITIONS}
    ${igeEffekseer_COMPILE_DEFINITIONS}
    ${igeNavigation_COMPILE_DEFINITIONS}
    ${igeNavigation_COMPILE_DEFINITIONS}
    ${igeSdk_COMPILE_DEFINITIONS}
)

target_compile_definitions(${TARGET_NAME} PUBLIC ${COMPILE_DEFINITIONS} Py_NO_ENABLE_SHARED USE_PYXCORE USE_IGECORE USE_IGESCENE USE_IGEVMATH)

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/app")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

#####################################
# Define Install Targets
#####################################
install(DIRECTORY "${CMAKE_SOURCE_DIR}/app/" DESTINATION ".")

install(TARGETS
  ${TARGET_NAME}
  RUNTIME DESTINATION bin
)

get_filename_component(PVRTEXLIB_LIB_DIR ${PVRTEXLIB_LIB} DIRECTORY)
install(DIRECTORY
    ${PVRTEXLIB_LIB_DIR}/
    DESTINATION bin
    FILES_MATCHING
        PATTERN "*.dll"
        PATTERN "*.so"
        PATTERN "*.dylib"
)

get_filename_component(FBX_LIB_DIR ${FBX_LIB} DIRECTORY)
install(DIRECTORY
    ${FBX_LIB_DIR}/
    DESTINATION bin
    FILES_MATCHING
        PATTERN "*.dll"
        PATTERN "*.so"
        PATTERN "*.dylib"
)

file(GLOB PVRTexLib_DLLS "${PVRTEXLIB_LIB_DIR}/*.dll")
foreach(dll IN LISTS PVRTexLib_DLLS)
    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMENT "Copying dll: ${dll}")
endforeach()

file(GLOB FBX_DLLS "${FBX_LIB_DIR}/*.dll")
foreach(dll IN LISTS FBX_DLLS)
    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMENT "Copying dll: ${dll}")
endforeach()
