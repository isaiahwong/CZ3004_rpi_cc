# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_SOURCE_DIR = /home/pi/Dev/CZ3004_rpi_c/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/Dev/CZ3004_rpi_c/build

# Include any dependencies generated for this target.
include Adder/CMakeFiles/adder.dir/depend.make

# Include the progress variables for this target.
include Adder/CMakeFiles/adder.dir/progress.make

# Include the compile flags for this target's objects.
include Adder/CMakeFiles/adder.dir/flags.make

Adder/CMakeFiles/adder.dir/adder.cc.o: Adder/CMakeFiles/adder.dir/flags.make
Adder/CMakeFiles/adder.dir/adder.cc.o: /home/pi/Dev/CZ3004_rpi_c/src/Adder/adder.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Dev/CZ3004_rpi_c/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Adder/CMakeFiles/adder.dir/adder.cc.o"
	cd /home/pi/Dev/CZ3004_rpi_c/build/Adder && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/adder.dir/adder.cc.o -c /home/pi/Dev/CZ3004_rpi_c/src/Adder/adder.cc

Adder/CMakeFiles/adder.dir/adder.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/adder.dir/adder.cc.i"
	cd /home/pi/Dev/CZ3004_rpi_c/build/Adder && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/pi/Dev/CZ3004_rpi_c/src/Adder/adder.cc > CMakeFiles/adder.dir/adder.cc.i

Adder/CMakeFiles/adder.dir/adder.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/adder.dir/adder.cc.s"
	cd /home/pi/Dev/CZ3004_rpi_c/build/Adder && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/pi/Dev/CZ3004_rpi_c/src/Adder/adder.cc -o CMakeFiles/adder.dir/adder.cc.s

# Object files for target adder
adder_OBJECTS = \
"CMakeFiles/adder.dir/adder.cc.o"

# External object files for target adder
adder_EXTERNAL_OBJECTS =

Adder/libadder.a: Adder/CMakeFiles/adder.dir/adder.cc.o
Adder/libadder.a: Adder/CMakeFiles/adder.dir/build.make
Adder/libadder.a: Adder/CMakeFiles/adder.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/Dev/CZ3004_rpi_c/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libadder.a"
	cd /home/pi/Dev/CZ3004_rpi_c/build/Adder && $(CMAKE_COMMAND) -P CMakeFiles/adder.dir/cmake_clean_target.cmake
	cd /home/pi/Dev/CZ3004_rpi_c/build/Adder && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/adder.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Adder/CMakeFiles/adder.dir/build: Adder/libadder.a

.PHONY : Adder/CMakeFiles/adder.dir/build

Adder/CMakeFiles/adder.dir/clean:
	cd /home/pi/Dev/CZ3004_rpi_c/build/Adder && $(CMAKE_COMMAND) -P CMakeFiles/adder.dir/cmake_clean.cmake
.PHONY : Adder/CMakeFiles/adder.dir/clean

Adder/CMakeFiles/adder.dir/depend:
	cd /home/pi/Dev/CZ3004_rpi_c/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/Dev/CZ3004_rpi_c/src /home/pi/Dev/CZ3004_rpi_c/src/Adder /home/pi/Dev/CZ3004_rpi_c/build /home/pi/Dev/CZ3004_rpi_c/build/Adder /home/pi/Dev/CZ3004_rpi_c/build/Adder/CMakeFiles/adder.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Adder/CMakeFiles/adder.dir/depend

