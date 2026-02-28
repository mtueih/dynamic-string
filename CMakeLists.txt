cmake_minimum_required(VERSION 4.1)
project(dynamic_string C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(deps/dynamic_array)

add_library(dstr STATIC src/dynamic_string.c)
target_include_directories(dstr PUBLIC include)
target_link_libraries(dstr PRIVATE darr)

add_executable(dynamic_string src/main.c)
target_link_libraries(dynamic_string PRIVATE dstr)

target_include_directories(dynamic_string PUBLIC include)
