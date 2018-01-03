cmake_minimum_required(VERSION 3.4.1)

set(
	PATH_TO_SUPERPOWERED
	CACHE STRING ""
)

message(${ANDROID_ABI})

file(GLOB CPP_FILES "*.cpp")
include_directories(${PATH_TO_SUPERPOWERED})

add_library(
            native-lib
            SHARED
            src/main/cpp/native-lib.cpp
)

add_library(
    SuperpoweredExample
    SHARED
    src/main/cpp/SuperpoweredExample.cpp
)

add_library(
	SuperpoweredAndroidAudioIO
	SHARED
	${CPP_FILES}
	${PATH_TO_SUPERPOWERED}/AndroidIO/SuperpoweredAndroidAudioIO.cpp
)

target_link_libraries(
    SuperpoweredExample
    SuperpoweredAndroidAudioIO
)

target_link_libraries(
    SuperpoweredAndroidAudioIO
    log
    android
    OpenSLES
    ${PATH_TO_SUPERPOWERED}/libSuperpoweredAndroid${ANDROID_ABI}.a
)

target_link_libraries(
    native-lib
    log
    android
)