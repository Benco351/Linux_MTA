cmake_minimum_required(VERSION 3.12)
project(LinuxAssignment2)

# Add the executable target
add_executable(LinuxAssignment2 main.c Utilities.c)

# Include the directory containing Utilities.h
target_include_directories(LinuxAssignment2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
