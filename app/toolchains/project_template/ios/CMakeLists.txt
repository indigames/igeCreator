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

# Preprocess App Configuration
set(PRODUCT_NAME #TARGET_NAME#)
set(EXECUTABLE_NAME ${PRODUCT_NAME})
set(BUNDLE_EXECUTABLE_NAME ${PRODUCT_NAME})
set(PRODUCT_BUNDLE_IDENTIFIER "#PACKAGE_NAME#")
set(BUNDLE_SHORT_VERSION_STRING "#VERSION_NAME#")
set(BUNDLE_VERSION "#VERSION_CODE#")
set(DEPLOYMENT_TARGET #DEPLOYMENT_TARGET#)
set(DEVICE_FAMILY "#DEVICE_FAMILY#")
set(DEVELOPMENT_TEAM_ID "#DEVELOPMENT_TEAM_ID#")
set(CODE_SIGN_IDENTITY "#CODE_SIGN_IDENTITY#")
set(PROVISIONING_PROFILE_NAME "#PROVISIONING_PROFILE_NAME#")

set(BUILD_PLATFORM ios)
set(BUILD_ARCH arm64)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED YES)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/LaunchScreen.storyboard
    ${CMAKE_CURRENT_SOURCE_DIR}/Assets.xcassets
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductIds.plist
    ${CMAKE_CURRENT_SOURCE_DIR}/.bundle
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/igeSdk/libs/iOS/Ketchapp.framework/Resources.bundle
)

if(CONAN_INCLUDE_DIRS_IGEFIREBASE)
    list(APPEND RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/GoogleService-Info.plist")
endif()

set(BUNDLES "")
foreach(LIB ${CONAN_LIB_DIRS})
    file(GLOB CHILD_LIBS RELATIVE ${LIB} ${LIB}/*)
    foreach(CHILD ${CHILD_LIBS})
        if(IS_DIRECTORY "${LIB}/${CHILD}")
            get_filename_component(CHILD_EXT "${CHILD}" EXT)
            if(CHILD_EXT MATCHES ".bundle")
                if(NOT "${LIB}/${CHILD}" IN_LIST BUNDLES)
                    list(APPEND BUNDLES "${LIB}/${CHILD}")
                endif()
            endif()
        endif()
    endforeach()
endforeach()

foreach(BUNDLE ${BUNDLES})
    list(APPEND RESOURCES ${BUNDLE})
endforeach()
add_executable(${TARGET_NAME} MACOSX_BUNDLE ${SRC_FILES} ${RESOURCES})

# Create the app target
set_target_properties(${TARGET_NAME} PROPERTIES
                      XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
                      XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/Prefix.pch"
                      RESOURCE "${RESOURCES}"
                      XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES"
                      XCODE_ATTRIBUTE_BUNDLE_EXECUTABLE_NAME ${BUNDLE_EXECUTABLE_NAME}
                      XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${PRODUCT_BUNDLE_IDENTIFIER}
                      XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET}
                      XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY ${DEVICE_FAMILY}
                      XCODE_ATTRIBUTE_BUNDLE_SHORT_VERSION_STRING ${BUNDLE_SHORT_VERSION_STRING}
                      XCODE_ATTRIBUTE_BUNDLE_VERSION ${BUNDLE_VERSION}
                      XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
                      MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
                      XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
                      XCODE_ATTRIBUTE_ENABLE_BITCODE NO
                      XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES YES
                      XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
                      XCODE_ATTRIBUTE_ENABLE_TESTABILITY NO
                      XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN YES
                      XCODE_ATTRIBUTE_ALWAYS_EMBED_SWIFT_STANDARD_LIBRARIES YES # Require for mediation networks
)
if (PROVISIONING_PROFILE_NAME STREQUAL "Automatic")
    set_target_properties(${TARGET_NAME} PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${DEVELOPMENT_TEAM_ID}
    )
else()
    set_target_properties(${TARGET_NAME} PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${CODE_SIGN_IDENTITY}
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${DEVELOPMENT_TEAM_ID}
        XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER ${PROVISIONING_PROFILE_NAME}
    )
endif()

# Set the app's linker search path to the default location on iOS
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$(inherited)")

# Locate system libraries on iOS
find_library(ACCELERATE Accelerate)
find_library(ADSUPPORT AdSupport)
find_library(APPTRACKINGTRANSPARENCY AppTrackingTransparency)
find_library(AUDIOTOOLBOX AudioToolBox)
find_library(AVFOUNDARION AVFoundation)
find_library(COREAUDIO CoreAudio)
find_library(COREFOUNDATION CoreFoundation)
find_library(COREGRAPHICS CoreGraphics)
find_library(COREMEDIA CoreMedia)
find_library(COREMOTION CoreMotion)
find_library(CORETELEPHONY CoreTelephony)
find_library(COREVIDEO CoreVideo)
find_library(FOUNDATION Foundation)
find_library(GAMEKIT GameKit)
find_library(GAMECONTROLLER GameController)
find_library(GLKIT GLKit)
find_library(MESSAGEUI MessageUI)
find_library(METAL Metal)
find_library(OPENGLES OpenGLES)
find_library(QUARTZCORE QuartzCore)
find_library(SAFARISERVICES SafariServices)
find_library(SECURITY Security)
find_library(STOREKIT StoreKit)
find_library(SYSTEMCONFIGURATION SystemConfiguration)
find_library(USERNOTIFICATION UserNotifications)

find_library(CFNETWORK CFNetwork)
find_library(MOBILECORESERVICES MobileCoreServices)
find_library(UIKIT UIKit)
find_library(PHOTOS Photos)
find_library(WEBKIT WebKit)

# link the frameworks located above
target_link_libraries(${TARGET_NAME} ${ACCELERATE})
target_link_libraries(${TARGET_NAME} ${ADSUPPORT})
target_link_libraries(${TARGET_NAME} ${APPTRACKINGTRANSPARENCY})
target_link_libraries(${TARGET_NAME} ${AUDIOTOOLBOX})
target_link_libraries(${TARGET_NAME} ${AVFOUNDARION})
target_link_libraries(${TARGET_NAME} ${COREAUDIO})
target_link_libraries(${TARGET_NAME} ${COREFOUNDATION})
target_link_libraries(${TARGET_NAME} ${COREGRAPHICS})
target_link_libraries(${TARGET_NAME} ${COREMEDIA})
target_link_libraries(${TARGET_NAME} ${COREMOTION})
target_link_libraries(${TARGET_NAME} ${CORETELEPHONY})
target_link_libraries(${TARGET_NAME} ${COREVIDEO})
target_link_libraries(${TARGET_NAME} ${FOUNDATION})
target_link_libraries(${TARGET_NAME} ${GAMEKIT})
target_link_libraries(${TARGET_NAME} ${GAMECONTROLLER})
target_link_libraries(${TARGET_NAME} ${GLKIT})
target_link_libraries(${TARGET_NAME} ${MESSAGEUI})
target_link_libraries(${TARGET_NAME} ${METAL})
target_link_libraries(${TARGET_NAME} ${OPENGLES})
target_link_libraries(${TARGET_NAME} ${QUARTZCORE})
target_link_libraries(${TARGET_NAME} ${SAFARISERVICES})
target_link_libraries(${TARGET_NAME} ${SECURITY})
target_link_libraries(${TARGET_NAME} ${STOREKIT})
target_link_libraries(${TARGET_NAME} ${SYSTEMCONFIGURATION})
target_link_libraries(${TARGET_NAME} ${USERNOTIFICATION})

target_link_libraries(${TARGET_NAME} ${CFNETWORK})
target_link_libraries(${TARGET_NAME} ${MOBILECORESERVICES})
target_link_libraries(${TARGET_NAME} ${UIKIT})
target_link_libraries(${TARGET_NAME} ${PHOTOS})
target_link_libraries(${TARGET_NAME} ${WEBKIT})

# For now, we don't use bluetooth
# find_library(COREBLUETOOTH CoreBluetooth)
# target_link_libraries(${TARGET_NAME} ${COREBLUETOOTH})

# Set output name
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "#TARGET_NAME#")
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "YES")

# Definitions
set(TARGET_DEFINITIONS Py_NO_ENABLE_SHARED)

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
target_compile_definitions(${TARGET_NAME} PUBLIC ${TARGET_DEFINITIONS})
set(XCODE_TARGET_DEFINITIONS "")
foreach(DEF ${TARGET_DEFINITIONS})
    string(APPEND XCODE_TARGET_DEFINITIONS " ${DEF}")
endforeach()
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PREPROCESSOR_DEFINITIONS "$(inherited)} ${XCODE_TARGET_DEFINITIONS}")

# Links
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_OTHER_LDFLAGS "$(inherited)")
set(LINKER_DIRECTORIES ${CMAKE_OUTPUT_DIR} ${CONAN_LIB_DIRS})
target_link_directories(${TARGET_NAME} PUBLIC ${LINKER_DIRECTORIES})
set(TARGET_LIB_DIRS "${CMAKE_OUTPUT_DIR}")
foreach(LIB ${CONAN_LIB_DIRS})
    string(APPEND TARGET_LIB_DIRS " ${LIB}")
endforeach()
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_LIBRARY_SEARCH_PATHS "$(inherited) ${TARGET_LIB_DIRS} $(PODS_CONFIGURATION_BUILD_DIR)")

target_link_libraries(${TARGET_NAME} ${CONAN_LIBS} z c++abi c++)

# Additional frameworks
set(TARGET_FRAMEWORK_DIRS "${CMAKE_OUTPUT_DIR}")
list(APPEND CONAN_LIB_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../libs/igeSdk/libs/iOS/")
foreach(LIB ${CONAN_LIB_DIRS})
    file(GLOB CHILD_LIBS RELATIVE ${LIB} ${LIB}/*)
    set(FRAMEWORKS "")
    foreach(CHILD ${CHILD_LIBS})
        if(IS_DIRECTORY ${LIB}/${CHILD})
            get_filename_component(CHILD_EXT ${CHILD} EXT)
            if(CHILD_EXT MATCHES ".framework")
                get_filename_component(FRAMEWORK_NAME ${CHILD} NAME_WE)
                list(APPEND FRAMEWORKS ${FRAMEWORK_NAME})
            endif()
        endif()
    endforeach()
    list(LENGTH FRAMEWORKS LEN_FRAMEWORKS)
    if(${LEN_FRAMEWORKS} GREATER 0)
        string(APPEND TARGET_FRAMEWORK_DIRS " ${LIB}")
        foreach(FRAMEWORK ${FRAMEWORKS})
            target_link_libraries(${TARGET_NAME} "-framework ${FRAMEWORK}")
        endforeach()
    endif()
endforeach()
set_target_properties(${TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_FRAMEWORK_SEARCH_PATHS "$(inherited) ${TARGET_FRAMEWORK_DIRS} $(PODS_CONFIGURATION_BUILD_DIR)")
