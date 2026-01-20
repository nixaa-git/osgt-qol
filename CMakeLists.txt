cmake_minimum_required(VERSION 3.12)
project(osgt-qol LANGUAGES CXX C)

# Only allow compiling via 64-bit MSVC.
if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Only 64-bit MSVC is supported.")
endif()

add_subdirectory(vendor)
add_subdirectory(src)
