cmake_minimum_required(VERSION 3.12)
project(IPC_Test)

# Add the executable target
add_executable(IPC_Test main.c Utilities.c)

# Include the directory containing Utilities.h
target_include_directories(IPC_Test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
