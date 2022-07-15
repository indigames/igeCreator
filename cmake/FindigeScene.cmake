# igeScene_FOUND
# igeScene_LIBRARIES
# igeScene_INCLUDE_DIRS

option(FORCE_BUILD_IGE "Force build IGE libraries" ON)

set(igeScene_FOUND 1)
set(igeScene_COMPILE_DEFINITIONS "-DUSE_IGESCENE")
set(igeScene_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../libs/igeScene")

if(NOT FORCE_BUILD_IGE AND USE_PREBUILT_LIBS)
    set(PLATFORM "windows")
    set(ARCH "x86_64")
    if(DEFINED ANDROID_PLATFORM)
        set(PLATFORM "android")
        if(ANDROID_ABI STREQUAL "armeabi-v7a")
            set(ARCH "armv7")
        elseif(ANDROID_ABI STREQUAL "x86")
            set(ARCH "x86")
        elseif(ANDROID_ABI STREQUAL "arm64-v8a")
            set(ARCH "armv8")
        elseif(ANDROID_ABI STREQUAL "x86_64")
            set(ARCH "x86_64")
        endif()
    elseif(MSVC)
        set(PLATFORM "windows")
        if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
            set(ARCH "x86_64")
        else()
            set(ARCH "x86")
        endif()
    elseif(EMSCRIPTEN)
        set(PLATFORM "emscripten")
        set(ARCH "wasm")
    elseif(APPLE)
        if(IOS)
            set(PLATFORM "ios")
            set(ARCH "arm8")
        else()
            set(PLATFORM "macos")
            set(ARCH "x86_64")
        endif()
    endif()    
    set(CMAKE_MODULE_PATH "${igeScene_BASE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
    include(FindigeBullet)
    include(FindigeCore)
    include(FindigeEffekseer)
    include(FindigeNavigation)
    include(FindigeSound)
    include(Findjson)
    set(igeScene_INCLUDE_DIRS "${igeScene_BASE_DIR}/prebuilt/${PLATFORM}/${ARCH}/include" ${json_INCLUDE_DIRS})
    set(igeScene_LIB_DIRS "${igeScene_BASE_DIR}/prebuilt/${PLATFORM}/${ARCH}/lib" ${igeBullet_LIB_DIRS} ${igeCore_LIB_DIRS} ${igeEffekseer_LIB_DIRS} ${igeNavigation_LIB_DIRS} ${igeSound_LIB_DIRS})
    set(igeScene_LIBRARIES "igeScene-static" ${igeBullet_LIBRARIES} ${igeCore_LIBRARIES} ${igeEffekseer_LIBRARIES} ${igeNavigation_LIBRARIES} ${igeSound_LIBRARIES})
else()
    if(NOT TARGET igeScene-static)
        add_subdirectory("${igeScene_BASE_DIR}" "${CMAKE_BINARY_DIR}/igeScene" EXCLUDE_FROM_ALL)
    endif()
    set(igeScene_INCLUDE_DIRS "${igeScene_BASE_DIR}")
    set(igeScene_LIBRARIES "igeScene-static")
endif()
