cmake_minimum_required(VERSION 3.14)
project(stb C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_subdirectory(stb)
add_subdirectory(tests)
