cmake_minimum_required(VERSION 3.4.1)

set(TARGET_NAME igeLauncher)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 17)

if(#DEBUG_BUILD#)
    option(USE_PREBUILT_LIBS "Use prebuilt libraries" OFF)
else()
    option(USE_PREBUILT_LIBS "Use prebuilt libraries" ON)
endif()

#CMAKE_DEPENDENCIES#

set(CONAN_DISABLE_CHECK_COMPILE TRUE)

# Preprocess App Configuration
set(PRODUCT_NAME #TARGET_NAME#)
set(EXECUTABLE_NAME ${PRODUCT_NAME})
set(BUNDLE_EXECUTABLE_NAME ${PRODUCT_NAME})
set(PRODUCT_BUNDLE_IDENTIFIER "#PACKAGE_NAME#")
set(BUNDLE_SHORT_VERSION_STRING "#VERSION_NAME#")
set(BUNDLE_VERSION "#VERSION_CODE#")

set(BUILD_PLATFORM macos)
set(BUILD_ARCH x64)

# Whether build from source or prebuilt
set (BUILD_ALL_SRC #DEBUG_BUILD#)
set (PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set (IGE_ENGINE_DIR ${PROJECT_ROOT_DIR}/../)

# Conan build config
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
    list(GET CONAN_INCLUDE_DIRS_PYXCORE 0 PYXCORE_INCLUDE_DIRS)
endif()

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Game.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/pythonExtension.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/pythonExtension.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/pythonStarter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/pythonStarter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Prefix.pch
)

set(RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductIds.plist
    ${CMAKE_CURRENT_SOURCE_DIR}/.bundle
)

if(CONAN_INCLUDE_DIRS_IGEFIREBASE)
    set(RESOURCES ${RESOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/GoogleService-Info.plist
    )
endif()

add_executable(${TARGET_NAME} MACOSX_BUNDLE ${SRC_FILES} ${RESOURCES})

# Create the app target
set_target_properties(${TARGET_NAME} PROPERTIES
                      XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
                      XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/Prefix.pch"
                      RESOURCE "${RESOURCES}"
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

# Set the app's linker search path to the default location on iOS
set_target_properties(
    ${TARGET_NAME}
    PROPERTIES
    XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS
    "@executable_path/Frameworks @loader_path/Frameworks"
)

# Locate system libraries on iOS
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

# link the frameworks located above
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

# Set output name
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "#TARGET_NAME#")
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "YES")

# Import other modules
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(ImportModules)

# Includes
set(TARGET_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
string(APPEND TARGET_INCLUDE_DIRS " ${PYXCORE_INCLUDE_DIRS}/pyxcore")
string(APPEND TARGET_INCLUDE_DIRS " ${PYXCORE_INCLUDE_DIRS}/ThirdParty/spdlog/include")
foreach(INC ${CONAN_INCLUDE_DIRS})
    string(APPEND TARGET_INCLUDE_DIRS " ${INC}")
endforeach()
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_HEADER_SEARCH_PATHS "$(inherited) ${TARGET_INCLUDE_DIRS}")

# Definitions
set(DEFINITIONS ${CONAN_DEFINES} Py_NO_ENABLE_SHARED)
target_compile_definitions(${TARGET_NAME} PUBLIC ${DEFINITIONS})
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PREPROCESSOR_DEFINITIONS "$(inherited)}")

# Links
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_OTHER_LDFLAGS "$(inherited)")
set(LINKER_DIRECTORIES ${CMAKE_OUTPUT_DIR} ${CONAN_LIB_DIRS})
target_link_directories(${TARGET_NAME} PUBLIC ${LINKER_DIRECTORIES})
set(TARGET_LIB_DIRS "${CMAKE_OUTPUT_DIR}")
foreach(LIB ${CONAN_LIB_DIRS})
    string(APPEND TARGET_LIB_DIRS " ${LIB}")
endforeach()
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_LIBRARY_SEARCH_PATHS "$(inherited) ${TARGET_LIB_DIRS} $(PODS_CONFIGURATION_BUILD_DIR)")

target_link_libraries(${TARGET_NAME} ${CONAN_LIBS} z c++abi c++ iconv)
