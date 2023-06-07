cmake_minimum_required(VERSION 3.22.1)
project(ourProj)
include_directories(${LIBZIP_INCLUDE_DIRS})
# Add the executable target
add_executable(ourProj main.c Utilities.c)

# Add the include directory
target_include_directories(ourProj PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ourProj /usr/lib/x86_64-linux-gnu/libzip.so)