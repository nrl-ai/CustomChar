include_directories(
    hnswlib
)
add_library(
    cc_embed_search
    embed_search.cpp
    test_emd.cpp
)
