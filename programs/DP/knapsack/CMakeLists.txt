cmake_minimum_required(VERSION 3.20)

project(knapsack LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP REQUIRED)

add_library(knapsack_lib
    src/knapsack.cpp
)

target_include_directories(knapsack_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(knapsack_lib
    PUBLIC
        OpenMP::OpenMP_CXX
)

target_compile_options(knapsack_lib PRIVATE
    -O3
    -march=native
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(knapsack
    src/main.cpp
)

target_link_libraries(knapsack
    PRIVATE knapsack_lib
)