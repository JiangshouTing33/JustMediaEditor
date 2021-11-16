# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

set(jnilibs ${CMAKE_SOURCE_DIR}/../libs/${ANDROID_ABI})
set(libname just-meditor)

include_directories(
        include
        glm
        inc
        inc/assimp
        inc/freetype_2_9_1
        decoder
        decoder/audio
        decoder/video
        encoder
        encoder/audio
        encoder/video
        muxer
        editor
        param
        render
        render/video
        render/video/sample
        render/video/sample/font
        render/video/sample/sticker
        render/video/sample/frame
        render/video/sample/mark
        render/audio
        render/audio/filter
        util)

file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/util/*.cpp
        ${CMAKE_SOURCE_DIR}/decoder/*.cpp
        ${CMAKE_SOURCE_DIR}/decoder/video/*.cpp
        ${CMAKE_SOURCE_DIR}/decoder/audio/*.cpp
        ${CMAKE_SOURCE_DIR}/encoder/*.cpp
        ${CMAKE_SOURCE_DIR}/encoder/video/*.cpp
        ${CMAKE_SOURCE_DIR}/encoder/audio/*.cpp
        ${CMAKE_SOURCE_DIR}/muxer/*.cpp
        ${CMAKE_SOURCE_DIR}/render/video/*.cpp
        ${CMAKE_SOURCE_DIR}/render/video/sample/*.cpp
        ${CMAKE_SOURCE_DIR}/render/video/sample/font/*.cpp
        ${CMAKE_SOURCE_DIR}/render/video/sample/sticker/*.cpp
        ${CMAKE_SOURCE_DIR}/render/video/sample/frame/*.cpp
        ${CMAKE_SOURCE_DIR}/render/video/sample/mark/*.cpp
        ${CMAKE_SOURCE_DIR}/render/audio/*.cpp
        ${CMAKE_SOURCE_DIR}/render/audio/filter/*.cpp
        ${CMAKE_SOURCE_DIR}/editor/*.cpp
        )

add_library( # Sets the name of the library.
        ${libname}

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${src-files}
        )

configure_file(${jnilibs}/libjustffmpeg.so ${Project_BINARY_DIR}/libjustffmpeg.so COPYONLY)

add_library(freetype STATIC IMPORTED )
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION ${jnilibs}/libfreetype.a)

## 添加ffmpeg相关的so库
add_library( libjustffmpeg
        SHARED
        IMPORTED )
set_target_properties( libjustffmpeg
        PROPERTIES IMPORTED_LOCATION
        libjustffmpeg.so )

set(native-libs
        android
        EGL
        GLESv3
        OpenSLES
        log
        m
        z
        )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(  # Specifies the target library.
                        ${libname}

                        # Links the target library to the log library
                        # included in the NDK.
                        ${log-lib}
                        freetype
                        libjustffmpeg
                        ${native-libs}
                        )