cmake_minimum_required(VERSION 4.1)
project(dynamic_string C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)


add_library(dstr STATIC src/dynamic_string.c)
target_include_directories(dstr PUBLIC include)

add_executable(dynamic_string src/main.c)
target_include_directories(dynamic_string PUBLIC include)
target_link_libraries(dynamic_string PRIVATE dstr)

