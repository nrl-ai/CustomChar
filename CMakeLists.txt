cmake_minimum_required(VERSION 3.11)
project(CustomChar)

# Version
set(CMAKE_PROJECT_VERSION 0)
set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 0)

# Using C++11
set(CMAKE_CXX_STANDARD 11)

option(WHISPER_SDL2 "" ON)
set(WHISPER_SDL2 ON)

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
    main.cpp
    libs/llama-cpp/llama.cpp
    libs/whisper-cpp/examples/common.cpp
    libs/whisper-cpp/examples/common-sdl.cpp
)
target_include_directories(
    ${TARGET} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    libs/llama-cpp
    libs/whisper-cpp
    libs/whisper-cpp/examples
)
target_link_libraries(${TARGET} PRIVATE ${SDL2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} whisper)
