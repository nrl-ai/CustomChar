cmake_minimum_required(VERSION 3.11)
project(CustomChar)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Version
set(CMAKE_PROJECT_VERSION 0)
set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 0)

# Using C++11
set(CMAKE_CXX_STANDARD 11)

# SDL2
find_package(SDL2 REQUIRED)
string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
message(STATUS "SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_LIBRARIES = ${SDL2_LIBRARIES}")

# Add whisper-cpp
add_subdirectory(libs/whisper-cpp)

# Talk with Llama V2
set(TARGET customchar)
add_executable(
    ${TARGET}
    customchar/main.cpp
    customchar/common.cpp
    customchar/llm.cpp
    customchar/speech_recognizer.cpp
    customchar/voice_synthesizer.cpp
    customchar/voice_recorder.cpp

    libs/llama-cpp/llama.cpp
    libs/whisper-cpp/examples/common.cpp
    libs/whisper-cpp/examples/common-sdl.cpp
)
target_include_directories(
    ${TARGET} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    libs
    .
)
target_link_libraries(${TARGET} PRIVATE ${SDL2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} whisper)
