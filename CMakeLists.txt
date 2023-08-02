cmake_minimum_required(VERSION 3.11)
project(CustomChar)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Version
set(CMAKE_PROJECT_VERSION 0)
set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 0)

# Using C++17
set(CMAKE_CXX_STANDARD 17)

# SDL2
find_package(SDL2 REQUIRED)
string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
message(STATUS "SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_LIBRARIES = ${SDL2_LIBRARIES}")

# OpenCV for perception module
find_package(OpenCV REQUIRED)

# Build submodules
add_subdirectory(libs/whisper-cpp)
add_subdirectory(libs/subprocess)
add_subdirectory(libs/SQLiteCpp)

include_directories(libs .)

# Build embedding search library
add_library(
    embeddb
    customchar/embeddb/document.cpp
    customchar/embeddb/embed_search.cpp
    customchar/embeddb/collection.cpp
)
target_link_libraries(
    embeddb
    SQLiteCpp
    sqlite3
    pthread
    dl
)

# Build CustomChar-core
set(TARGET customchar-core)
add_library(
    ${TARGET}
    customchar/character/character.cpp
    customchar/common/common.cpp
    customchar/common/helpers.cpp
    customchar/llm/llm.cpp
    customchar/audio/speech_recognizer.cpp
    customchar/audio/voice_synthesizer.cpp
    customchar/audio/voice_recorder.cpp
    customchar/audio/audio.cpp
    customchar/audio/sdl.cpp
    customchar/executors/plugin_executor.cpp
    libs/llama-cpp/llama.cpp
)
target_include_directories(
    ${TARGET} PUBLIC
    ${SDL2_INCLUDE_DIRS}
)
target_link_libraries(${TARGET} PUBLIC ${SDL2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS} whisper subprocess embeddb)

add_executable(
    search_doc
    examples/search_doc.cpp
)
target_link_libraries(search_doc PUBLIC customchar-core)

# CustomChar - cli
add_executable(
    customchar-cli
    customchar/main_cli.cpp
)
target_link_libraries(customchar-cli customchar-core)

option(BUILD_GUI "Build GUI" ON)

if(BUILD_GUI)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3 REQUIRED)

    include_directories(libs/imgui/include)
    set(IMGUI_DIR libs/imgui)
    set(IMGUI_SRCS
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )

    add_library(imgui STATIC ${IMGUI_SRCS})
    target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)

    if(UNIX AND NOT APPLE)
        message(STATUS "Building for Linux")
        set(LINUX_GL_LIBS GL GLEW)
        target_link_libraries(${TARGET} PUBLIC ${LINUX_GL_LIBS} glfw)
        target_compile_definitions(${TARGET} PUBLIC LINUX)
    elseif(APPLE)
        message(STATUS "Building for Mac OS X")
        target_link_libraries(${TARGET} PUBLIC "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" glfw)
        target_compile_definitions(${TARGET} PUBLIC APPLE)
        include_directories(/usr/local/include /opt/local/include /opt/homebrew/include)
    else()
        message(STATUS "Building for Windows")
        target_link_libraries(${TARGET} PUBLIC glfw opengl32 imm32)
        target_compile_definitions(${TARGET} PUBLIC WINDOWS)
    endif()

    add_executable(
        customchar
        customchar/main.cpp
        customchar/session/chat_history.cpp
        customchar/session/chat_message.cpp
    )
    target_link_libraries(customchar customchar-core imgui)

    # Copy the fonts to the build directory
    add_custom_command(TARGET customchar POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:customchar>/fonts)
endif()

add_executable(
    test_ffmpeg_as_input tests/test_ffmpeg_as_input.cpp
)
target_link_libraries(test_ffmpeg_as_input ${OpenCV_LIBS})
