project(customchar-cpp)
cmake_minimum_required(VERSION 3.22)

# Using C++17
set(CMAKE_CXX_STANDARD 17)

option(WHISPER_SDL2 "" ON)
set(WHISPER_SDL2 ON)

# SDL2
find_package(SDL2 REQUIRED)
string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
message(STATUS "SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_LIBRARIES = ${SDL2_LIBRARIES}")

add_subdirectory(whisper-cpp)

set(TARGET try-customchar)
add_executable(${TARGET} try-customchar.cpp llama.cpp whisper-cpp/examples/common.cpp whisper-cpp/examples/common-sdl.cpp)

target_include_directories(${TARGET} PRIVATE ${SDL2_INCLUDE_DIRS} whisper-cpp/examples whisper-cpp)
target_link_libraries(${TARGET} PRIVATE ${SDL2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} whisper)
