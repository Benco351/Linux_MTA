# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/build"

# Include any dependencies generated for this target.
include CMakeFiles/FlightService.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/FlightService.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/FlightService.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/FlightService.dir/flags.make

CMakeFiles/FlightService.dir/FlightService.c.o: CMakeFiles/FlightService.dir/flags.make
CMakeFiles/FlightService.dir/FlightService.c.o: ../FlightService.c
CMakeFiles/FlightService.dir/FlightService.c.o: CMakeFiles/FlightService.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/FlightService.dir/FlightService.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/FlightService.dir/FlightService.c.o -MF CMakeFiles/FlightService.dir/FlightService.c.o.d -o CMakeFiles/FlightService.dir/FlightService.c.o -c "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/FlightService.c"

CMakeFiles/FlightService.dir/FlightService.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FlightService.dir/FlightService.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/FlightService.c" > CMakeFiles/FlightService.dir/FlightService.c.i

CMakeFiles/FlightService.dir/FlightService.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FlightService.dir/FlightService.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/FlightService.c" -o CMakeFiles/FlightService.dir/FlightService.c.s

CMakeFiles/FlightService.dir/Utilities.c.o: CMakeFiles/FlightService.dir/flags.make
CMakeFiles/FlightService.dir/Utilities.c.o: ../Utilities.c
CMakeFiles/FlightService.dir/Utilities.c.o: CMakeFiles/FlightService.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/FlightService.dir/Utilities.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/FlightService.dir/Utilities.c.o -MF CMakeFiles/FlightService.dir/Utilities.c.o.d -o CMakeFiles/FlightService.dir/Utilities.c.o -c "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/Utilities.c"

CMakeFiles/FlightService.dir/Utilities.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FlightService.dir/Utilities.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/Utilities.c" > CMakeFiles/FlightService.dir/Utilities.c.i

CMakeFiles/FlightService.dir/Utilities.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FlightService.dir/Utilities.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/Utilities.c" -o CMakeFiles/FlightService.dir/Utilities.c.s

# Object files for target FlightService
FlightService_OBJECTS = \
"CMakeFiles/FlightService.dir/FlightService.c.o" \
"CMakeFiles/FlightService.dir/Utilities.c.o"

# External object files for target FlightService
FlightService_EXTERNAL_OBJECTS =

FlightService: CMakeFiles/FlightService.dir/FlightService.c.o
FlightService: CMakeFiles/FlightService.dir/Utilities.c.o
FlightService: CMakeFiles/FlightService.dir/build.make
FlightService: /usr/lib/x86_64-linux-gnu/libzip.so
FlightService: CMakeFiles/FlightService.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable FlightService"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FlightService.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/FlightService.dir/build: FlightService
.PHONY : CMakeFiles/FlightService.dir/build

CMakeFiles/FlightService.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/FlightService.dir/cmake_clean.cmake
.PHONY : CMakeFiles/FlightService.dir/clean

CMakeFiles/FlightService.dir/depend:
	cd "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService" "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService" "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/build" "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/build" "/home/bens/Desktop/github/Linux_MTA/Assignment#3/FlightsService/build/CMakeFiles/FlightService.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/FlightService.dir/depend
