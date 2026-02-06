add_library(stb_arena STATIC stb_arena.c)

target_include_directories(stb_arena PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
