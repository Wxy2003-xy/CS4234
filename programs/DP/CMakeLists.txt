cmake_minimum_required(VERSION 3.20)

project(dynamic_programming LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Header-only problem implementations. CONFIGURE_DEPENDS makes CMake notice
# newly added .hpp problem files when the project is rebuilt.
file(GLOB DP_PROBLEM_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_executable(dp
    src/main.cpp
    ${DP_PROBLEM_HEADERS}
)

target_include_directories(dp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(dp PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Rebuild changed files and run all demos:
#   cmake --build build --target run
add_custom_target(run
    COMMAND $<TARGET_FILE:dp> all
    DEPENDS dp
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
    COMMENT "Running dynamic-programming demos"
)
