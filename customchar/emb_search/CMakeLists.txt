set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set (
    SOURCES
    embed_search.cpp
    test_emd.cpp
)


add_executable(test_search_embed ${SOURCES})
# target_link_libraries(test_search_embed customchar)
