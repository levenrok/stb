include(FetchContent)

FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG        v2.6.1
)
FetchContent_MakeAvailable(unity)

add_executable(arena_test
    arena_test.c
)

target_link_libraries(arena_test unity)
target_include_directories(arena_test PRIVATE ../stb)

add_test(NAME ArenaTest COMMAND arena_test)
