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

# Build options
option(BUILD_DOCS "Build documentation" OFF)

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

if(BUILD_DOCS)
    # ==================================================
    # Doxygen - For documentation
    # ==================================================
    # Look for Doxygen package
    find_package(Doxygen)

    if(DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # Note: do not put "ALL" - this builds docs together with application EVERY TIME!
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    else(DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif(DOXYGEN_FOUND)
endif()
