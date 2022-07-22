cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0091 NEW)

set(TARGET_NAME igeCreator)

project(${TARGET_NAME})

option(USE_PREBUILT_LIBS "Use prebuilt libraries" ON)
option(FORCE_BUILD_IGECORE "Force build igeCore" ON)
option(FORCE_BUILD_PYXCORE "Force build pyxCore" ON)
option(FORCE_BUILD_IGESCENE "Force build igeScene" ON)
option(FORCE_BUILD_IGEVMATH "Force build igeVmath" ON)
option(FORCE_BUILD_IGESDK "Force build igeSdk" ON)

if(USE_PREBUILT_LIBS)
    set(USE_MSVC_RELEASE_RUNTIME_ALWAYS ON)
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 0)
set(CMAKE_PROJECT_VERSION ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH})

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/../pyxCore/pyxtools/cmake"
)
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
set(CPACK_PACKAGE_VERSION "v${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_GENERATOR "ZIP;NSIS")
set(CPACK_STRIP_FILES "TRUE")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_LICENSE_PROVIDED "1")
set(CPACK_NSIS_CONTACT "admin@indigames.net")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/igeCreator.ico")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_CURRENT_SOURCE_DIR}/src/igeCreator.ico")
set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${TARGET_NAME}.lnk' '$INSTDIR\\\\bin\\\\${TARGET_NAME}.exe'")
set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '$SMPROGRAMS\\\\$START_MENU\\\\${TARGET_NAME}.lnk'")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "CreateShortCut \\\"$DESKTOP\\\\${TARGET_NAME}.lnk\\\" \\\"$INSTDIR\\\\bin\\\\${TARGET_NAME}.exe\\\"")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "Delete \\\"$DESKTOP\\\\${TARGET_NAME}.lnk\\\"")
include(CPack)

add_compile_options(/MP)
add_compile_definitions(VERSION_NAME="${CMAKE_PROJECT_VERSION}" EDITOR_MODE=1 IMGUI_DEFINE_MATH_OPERATORS=1)
add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>)

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


include(pyxCore)
include(igeCore)
include(igeVmath)
include(igeScene)
include(igeBullet)
include(igeSound)
include(igeEffekseer)
include(igeNavigation)
include(igeSdk)

if(${APP_STYLE} MATCHES "SHARED")
    find_package (Python3 ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Development)
else()
    find_package(Python REQUIRED)
    set (Python3_INCLUDE_DIRS ${Python_INCLUDE_DIRS})
    set (Python3_LIB_DIRS ${Python_LIB_DIRS})
    set (Python3_LIBRARIES ${Python_LIBRARIES})
endif()

find_package(numpy REQUIRED)
find_package(stb REQUIRED)
find_package(SDL REQUIRED)
find_package(openssl REQUIRED)
find_package(taskflow REQUIRED)
# find_package(tracy REQUIRED)
find_package(zlib REQUIRED)
find_package(json REQUIRED)

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

target_include_directories(${TARGET_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/pfd"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/ImGuizmo"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/imgui-node-editor"
    "${igeVmath_INCLUDE_DIRS}"
    "${pyxCore_INCLUDE_DIRS}"
    "${igeCore_INCLUDE_DIRS}"
    "${igeScene_INCLUDE_DIRS}"
    "${igeBullet_INCLUDE_DIRS}"
    "${igeSound_INCLUDE_DIRS}"
    "${igeEffekseer_INCLUDE_DIRS}"
    "${igeNavigation_INCLUDE_DIRS}"
    "${igeSdk_INCLUDE_DIRS}"
    "${Python3_INCLUDE_DIRS}"
    "${numpy_INCLUDE_DIRS}"
    "${zlib_INCLUDE_DIRS}"
    "${openssl_INCLUDE_DIRS}"
    "${json_INCLUDE_DIRS}"    
    "${stb_INCLUDE_DIRS}"
    "${SDL_INCLUDE_DIRS}"    
    "${taskflow_INCLUDE_DIRS}"
    "${tracy_INCLUDE_DIRS}"
)

set(COMPILE_DEFINITIONS
    ${pyxCore_COMPILE_DEFINITIONS}
    ${igeCore_COMPILE_DEFINITIONS}
    ${igeVmath_COMPILE_DEFINITIONS}
    ${igeSceme_COMPILE_DEFINITIONS}
    ${igeBullet_COMPILE_DEFINITIONS}
    ${igeSound_COMPILE_DEFINITIONS}
    ${igeEffekseer_COMPILE_DEFINITIONS}
    ${igeNavigation_COMPILE_DEFINITIONS}
    ${igeSdk_COMPILE_DEFINITIONS}
)
target_compile_definitions(${TARGET_NAME} PUBLIC ${COMPILE_DEFINITIONS} Py_NO_ENABLE_SHARED)

target_link_directories(${TARGET_NAME} PUBLIC
    "${igeVmath_LIB_DIRS}"
    "${pyxCore_LIB_DIRS}"
    "${igeCore_LIB_DIRS}"
    "${igeScene_LIB_DIRS}"
    "${igeBullet_LIB_DIRS}"
    "${igeSound_LIB_DIRS}"
    "${igeEffekseer_LIB_DIRS}"
    "${igeNavigation_LIB_DIRS}"
    "${igeSdk_LIB_DIRS}"
    "${Python3_LIB_DIRS}"
    "${numpy_LIB_DIRS}"
    "${zlib_LIB_DIRS}"
    "${openssl_LIB_DIRS}"
    "${json_LIB_DIRS}"    
    "${stb_LIB_DIRS}"
    "${SDL_LIB_DIRS}"    
    "${taskflow_LIB_DIRS}"
    "${tracy_LIB_DIRS}"
)

target_link_libraries(${TARGET_NAME}
    ${igeVmath_LIBRARIES}
    ${pyxCore_LIBRARIES}
    ${igeCore_LIBRARIES}
    ${igeScene_LIBRARIES}
    ${igeBullet_LIBRARIES}
    ${igeSound_LIBRARIES}
    ${igeEffekseer_LIBRARIES}
    ${igeNavigation_LIBRARIES}
    ${igeSdk_LIBRARIES}
    ${Python3_LIBRARIES}
    ${numpy_LIBRARIES}
    ${zlib_LIBRARIES}
    ${openssl_LIBRARIES}
    ${json_LIBRARIES}
    ${stb_LIBRARIES}
    ${SDL_LIBRARIES}
    ${taskflow_LIBRARIES}
    ${tracy_LIBRARIES}
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

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/app"
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099 /ignore:4286 /ignore:4127") # Disable linker warning
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libvcruntime /NODEFAULTLIB:libvcruntimed>")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:python37 /NODEFAULTLIB:python39")

#####################################
# Define Install Targets
#####################################
install(DIRECTORY "${CMAKE_SOURCE_DIR}/app/" DESTINATION ".")

install(TARGETS
  ${TARGET_NAME}
  RUNTIME DESTINATION bin
)

FIND_PACKAGE(PVRTexLib REQUIRED)
get_filename_component(PVRTEXLIB_LIB_DIR ${PVRTEXLIB_LIB} DIRECTORY)
install(DIRECTORY
    ${PVRTEXLIB_LIB_DIR}/
    DESTINATION bin
    FILES_MATCHING
        PATTERN "*.dll"
        PATTERN "*.so"
        PATTERN "*.dylib"
)

FIND_PACKAGE(fbx REQUIRED)
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
