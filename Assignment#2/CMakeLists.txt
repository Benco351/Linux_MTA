cmake_minimum_required(VERSION 3.12)
project(ourProj)

# Add the executable target
add_executable(ourProj main.c Utilities.c)

# Add the include directory
target_include_directories(ourProj PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})