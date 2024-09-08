cmake_minimum_required(VERSION 3.12)
project(osgt-qol LANGUAGES CXX C)

if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Only 64-bit MSVC is supported.")
endif()

# Set sources.
set (SOURCES_DINPUT8
    src/dinput8/dinput8.cpp
    src/dinput8/dinput8.def
)

add_library(dinput8 SHARED ${SOURCES_DINPUT8})
# Don't use /W4, it's too noisy.
target_compile_options(dinput8 PRIVATE /W3)
target_link_options(dinput8 PRIVATE)