cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0091 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(TARGET_NAME igeCreator)

project(${TARGET_NAME})

option(USE_PREBUILT_LIBS "Use prebuilt libraries" ON)
option(FORCE_BUILD_IGECORE "Force build igeCore" ON)
option(FORCE_BUILD_PYXCORE "Force build pyxCore" ON)
option(FORCE_BUILD_IGESCENE "Force build igeScene" ON)
option(FORCE_BUILD_IGEVMATH "Force build igeVmath" ON)
option(FORCE_BUILD_IGESDK "Force build igeSdk" ON)

if(MSVC)
    if(USE_PREBUILT_LIBS)
        set(USE_MSVC_RELEASE_RUNTIME_ALWAYS ON)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
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
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set the Relative Path Configurations
if(MSVC)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

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
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_LICENSE_PROVIDED "1")
if(MSVC)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_STRIP_FILES "TRUE")
    set(CPACK_NSIS_CONTACT "admin@indigames.net")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/igeCreator.ico")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
    set(CPACK_NSIS_MODIFY_PATH "ON")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_CURRENT_SOURCE_DIR}/src/igeCreator.ico")
    list(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${TARGET_NAME}.lnk' '$INSTDIR\\\\bin\\\\${TARGET_NAME}.exe'")
    list(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA " Delete '$SMPROGRAMS\\\\$START_MENU\\\\${TARGET_NAME}.lnk'")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP;DragNDrop")
    set(CMAKE_MACOSX_RPATH TRUE)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++17" "-stdlib=libc++")
    set(PRODUCT_NAME "igeCreator")
    set(EXECUTABLE_NAME ${PRODUCT_NAME})
    set(BUNDLE_EXECUTABLE_NAME ${PRODUCT_NAME})
    set(PRODUCT_BUNDLE_IDENTIFIER "net.indigames.igecreator")
    set(BUNDLE_SHORT_VERSION_STRING "${CMAKE_PROJECT_VERSION}")
    set(BUNDLE_VERSION "${CMAKE_PROJECT_VERSION_MAJOR}${CMAKE_PROJECT_VERSION_MINOR}${CMAKE_PROJECT_VERSION_PATCH}")
endif()
include(CPack)

add_compile_definitions(EDITOR_MODE=1 IMGUI_DEFINE_MATH_OPERATORS=1)

include(compiler_options)

if(MSVC)
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

find_package(Python REQUIRED)
set (Python3_INCLUDE_DIRS ${Python_INCLUDE_DIRS})
set (Python3_LIB_DIRS ${Python_LIB_DIRS})
set (Python3_LIBRARIES ${Python_LIBRARIES})

find_package(numpy REQUIRED)
find_package(stb REQUIRED)
find_package(SDL REQUIRED)
find_package(openssl REQUIRED)
find_package(zlib REQUIRED)
find_package(json REQUIRED)

find_package(PVRTexLib REQUIRED)
find_package(fbx REQUIRED)


# Source files
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.mm"
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

if(MSVC)
    set(XC_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/igeCreator.rc")
elseif(APPLE)
    list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    list(APPEND SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.mm")

    set(XC_RESOURCES "")
    file(GLOB RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/app/*)
    foreach(RES ${RESOURCES})
        list(APPEND XC_RESOURCES "${RES}")
    endforeach()
    set_source_files_properties(${XC_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES} ${XC_RESOURCES})


if(MSVC)
    add_executable(${TARGET_NAME} WIN32 ${SRC_FILES} ${XC_RESOURCES})
elseif(APPLE)
    add_executable(${TARGET_NAME} MACOSX_BUNDLE ${SRC_FILES} ${XC_RESOURCES})
endif()

set(TARGET_INCLUDE_DIRS
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
)
target_include_directories(${TARGET_NAME} PUBLIC ${TARGET_INCLUDE_DIRS})

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
    Py_NO_ENABLE_SHARED
    USE_NUMPY
    EDITOR_MODE=1    
    VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR}
    VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR}
    VERSION_PATCH=${CMAKE_PROJECT_VERSION_PATCH}
    IMGUI_DEFINE_MATH_OPERATORS=1
    GL_SILENCE_DEPRECATION
)
target_compile_definitions(${TARGET_NAME} PUBLIC ${COMPILE_DEFINITIONS})

set(TARGET_LIB_DIRS
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
    "${PVRTEXLIB_LIB_DIRS}"
    "${FBX_LIB_DIRS}"
)
target_link_directories(${TARGET_NAME} PUBLIC ${TARGET_LIB_DIRS})

set(TARGET_LIBS
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
    ${PVRTEXLIB_LIBRARIES}
    ${FBX_LIBRARIES}
)
target_link_libraries(${TARGET_NAME} ${TARGET_LIBS})

if(MSVC)
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
    set_target_properties(
        ${TARGET_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/app"
    )
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099 /ignore:4286 /ignore:4127") # Disable linker warning
    if(USE_PREBUILT_LIBS)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libvcruntime /NODEFAULTLIB:libvcruntimed")
    endif()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:python37 /NODEFAULTLIB:python39")
elseif(APPLE)
    set_target_properties(${TARGET_NAME} PROPERTIES
        XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
        XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/Prefix.pch"
        # RESOURCE "${XC_RESOURCES}"
        XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${PRODUCT_BUNDLE_IDENTIFIER}
        XCODE_ATTRIBUTE_BUNDLE_SHORT_VERSION_STRING ${BUNDLE_SHORT_VERSION_STRING}
        XCODE_ATTRIBUTE_BUNDLE_VERSION ${BUNDLE_VERSION}
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES YES
        XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
        XCODE_ATTRIBUTE_ENABLE_TESTABILITY NO
        XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN YES
    )
    set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic")
    set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/Frameworks @loader_path/Frameworks")
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${PRODUCT_NAME})
    set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "YES")
    set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_COPY_PHASE_STRIP "NO")
    set_target_properties(${TARGET_NAME} PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@rpath")

    set(XC_INCLUDE_DIRS "")
    foreach(DIRS ${TARGET_INCLUDE_DIRS})
        foreach(INC ${DIRS})
            string(APPEND XC_INCLUDE_DIRS " ${INC}")
        endforeach()
    endforeach()
    set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_HEADER_SEARCH_PATHS "$(inherited) ${XC_INCLUDE_DIRS}")

    set(XC_DEFINES "")
    foreach(DEF ${COMPILE_DEFINITIONS})
        string(REPLACE "-D" "" DEF ${DEF})
        string(APPEND XC_DEFINES " ${DEF}")
    endforeach()
    set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PREPROCESSOR_DEFINITIONS "${XC_DEFINES} $(inherited)")

    set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_OTHER_LDFLAGS "$(inherited)")
    set(LINKER_DIRECTORIES ${CMAKE_OUTPUT_DIR} ${TARGET_LIB_DIRS})
    target_link_directories(${TARGET_NAME} PUBLIC ${LINKER_DIRECTORIES})
    set(TARGET_LIB_DIRS "${CMAKE_OUTPUT_DIR}")
    foreach(LIB ${LINKER_DIRECTORIES})
        string(APPEND TARGET_LIB_DIRS " ${LIB}")
    endforeach()
    set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_LIBRARY_SEARCH_PATHS "$(inherited) ${TARGET_LIB_DIRS} $(PODS_CONFIGURATION_BUILD_DIR)")
    target_link_libraries(${TARGET_NAME} ${CONAN_LIBS} z c++abi c++ iconv)
    find_library(ACCELERATE Accelerate)
    find_library(AUDIOTOOLBOX AudioToolBox)
    find_library(AVFOUNDARION AVFoundation)
    find_library(COREAUDIO CoreAudio)
    find_library(COREBLUETOOTH CoreBluetooth)
    find_library(COREFOUNDATION CoreFoundation)
    find_library(COREGRAPHICS CoreGraphics)
    find_library(COREMEDIA CoreMedia)
    find_library(COREMOTION CoreMotion)
    find_library(COREVIDEO CoreVideo)
    find_library(COREHAPTICS CoreHaptics)
    find_library(FOUNDATION Foundation)
    find_library(GAMEKIT GameKit)
    find_library(GAMECONTROLLER GameController)
    find_library(GLKIT GLKit)
    find_library(OPENGLES OpenGL)
    find_library(QUARTZCORE QuartzCore)
    find_library(SECURITY Security)
    find_library(SYSTEMCONFIGURATION SystemConfiguration)
    find_library(USERNOTIFICATION UserNotifications)
    find_library(CFNETWORK CFNetwork)
    find_library(CARBON Carbon)
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)
    find_library(FORCEFEEDBACK ForceFeedback)
    find_library(GSS GSS)
    find_library(KERBEROS Kerberos)
    find_library(OPENCL OpenCL)
    find_library(METAL Metal)
    target_link_libraries(${TARGET_NAME} ${ACCELERATE})
    target_link_libraries(${TARGET_NAME} ${AUDIOTOOLBOX})
    target_link_libraries(${TARGET_NAME} ${AVFOUNDARION})
    target_link_libraries(${TARGET_NAME} ${COREAUDIO})
    target_link_libraries(${TARGET_NAME} ${COREBLUETOOTH})
    target_link_libraries(${TARGET_NAME} ${COREFOUNDATION})
    target_link_libraries(${TARGET_NAME} ${COREGRAPHICS})
    target_link_libraries(${TARGET_NAME} ${COREMEDIA})
    target_link_libraries(${TARGET_NAME} ${COREMOTION})
    target_link_libraries(${TARGET_NAME} ${COREVIDEO})
    target_link_libraries(${TARGET_NAME} ${COREHAPTICS})
    target_link_libraries(${TARGET_NAME} ${FOUNDATION})
    target_link_libraries(${TARGET_NAME} ${GAMEKIT})
    target_link_libraries(${TARGET_NAME} ${GAMECONTROLLER})
    target_link_libraries(${TARGET_NAME} ${GLKIT})
    target_link_libraries(${TARGET_NAME} ${OPENGLES})
    target_link_libraries(${TARGET_NAME} ${QUARTZCORE})
    target_link_libraries(${TARGET_NAME} ${SECURITY})
    target_link_libraries(${TARGET_NAME} ${SYSTEMCONFIGURATION})
    target_link_libraries(${TARGET_NAME} ${USERNOTIFICATION})
    target_link_libraries(${TARGET_NAME} ${CFNETWORK})
    target_link_libraries(${TARGET_NAME} ${CARBON})
    target_link_libraries(${TARGET_NAME} ${COCOA})
    target_link_libraries(${TARGET_NAME} ${IOKIT})
    target_link_libraries(${TARGET_NAME} ${FORCEFEEDBACK})
    target_link_libraries(${TARGET_NAME} ${GSS})
    target_link_libraries(${TARGET_NAME} ${KERBEROS})
    target_link_libraries(${TARGET_NAME} ${OPENCL})
    target_link_libraries(${TARGET_NAME} ${METAL})
endif()

#####################################
# Define Install Targets
#####################################
if(MSVC)
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/app/" DESTINATION ".")

    install(TARGETS
        ${TARGET_NAME}
        RUNTIME DESTINATION bin
    )

    file(GLOB PVRTexLib_DLLS "${PVRTEXLIB_LIB_DIRS}/*.dll")
    foreach(dll IN LISTS PVRTexLib_DLLS)
        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMENT "Copying dll: ${dll}")
    endforeach()

    file(GLOB FBX_DLLS "${FBX_LIB_DIRS}/*.dll")
    foreach(dll IN LISTS FBX_DLLS)
        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMENT "Copying dll: ${dll}")
    endforeach()
    install(DIRECTORY
        ${PVRTEXLIB_LIB_DIRS}/
        DESTINATION bin
        FILES_MATCHING
            PATTERN "*.dll"
            PATTERN "*.so"
            PATTERN "*.dylib"
    )
    install(DIRECTORY
        ${FBX_LIB_DIRS}/
        DESTINATION bin
        FILES_MATCHING
            PATTERN "*.dll"
            PATTERN "*.so"
            PATTERN "*.dylib"
    )
elseif(OSX)
    get_filename_component(PVRTEXLIB_LIB_DIR ${PVRTEXLIB_LIB} DIRECTORY)
    install(DIRECTORY
        ${PVRTEXLIB_LIB_DIR}/
        DESTINATION "$<TARGET_FILE_DIR:${TARGET_NAME}>/../lib"
        FILES_MATCHING
            PATTERN "*.so"
            PATTERN "*.dylib"
    )

    get_filename_component(FBX_LIB_DIR ${FBX_LIB} DIRECTORY)
    install(DIRECTORY
        ${FBX_LIB_DIR}/
        DESTINATION "$<TARGET_FILE_DIR:${TARGET_NAME}>"
        FILES_MATCHING
            PATTERN "*.so"
            PATTERN "*.dylib"
    )

    install(TARGETS ${TARGET_NAME} BUNDLE DESTINATION ".")
endif()
