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
add_subdirectory(whisper-cpp)

# Add executable try-customchar
set(TARGET try-customchar)
add_executable(${TARGET} try-customchar.cpp llama.cpp whisper-cpp/examples/common.cpp whisper-cpp/examples/common-sdl.cpp)
target_include_directories(${TARGET} PRIVATE ${SDL2_INCLUDE_DIRS} whisper-cpp/examples whisper-cpp)
target_link_libraries(${TARGET} PRIVATE ${SDL2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} whisper)
