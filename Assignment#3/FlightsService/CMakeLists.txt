cmake_minimum_required(VERSION 3.22.1)
project(FlightService)
include_directories(${LIBZIP_INCLUDE_DIRS})
# Add the executable target
add_executable(FlightService FlightService.c Utilities.c)

# Add the include directory
target_include_directories(FlightService PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
find_library(LIBZIP_LIBRARY NAMES zip)
target_link_libraries(FlightService ${LIBZIP_LIBRARY} rt)
