# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/bustub

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/bustub/cmake-build-debug_wsl

# Include any dependencies generated for this target.
include googletest-build/googlemock/CMakeFiles/gmock_main.dir/depend.make

# Include the progress variables for this target.
include googletest-build/googlemock/CMakeFiles/gmock_main.dir/progress.make

# Include the compile flags for this target's objects.
include googletest-build/googlemock/CMakeFiles/gmock_main.dir/flags.make

googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o: googletest-build/googlemock/CMakeFiles/gmock_main.dir/flags.make
googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o: googletest-src/googlemock/src/gmock_main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/bustub/cmake-build-debug_wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o"
	cd /mnt/d/bustub/cmake-build-debug_wsl/googletest-build/googlemock && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gmock_main.dir/src/gmock_main.cc.o -c /mnt/d/bustub/cmake-build-debug_wsl/googletest-src/googlemock/src/gmock_main.cc

googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gmock_main.dir/src/gmock_main.cc.i"
	cd /mnt/d/bustub/cmake-build-debug_wsl/googletest-build/googlemock && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/bustub/cmake-build-debug_wsl/googletest-src/googlemock/src/gmock_main.cc > CMakeFiles/gmock_main.dir/src/gmock_main.cc.i

googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gmock_main.dir/src/gmock_main.cc.s"
	cd /mnt/d/bustub/cmake-build-debug_wsl/googletest-build/googlemock && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/bustub/cmake-build-debug_wsl/googletest-src/googlemock/src/gmock_main.cc -o CMakeFiles/gmock_main.dir/src/gmock_main.cc.s

# Object files for target gmock_main
gmock_main_OBJECTS = \
"CMakeFiles/gmock_main.dir/src/gmock_main.cc.o"

# External object files for target gmock_main
gmock_main_EXTERNAL_OBJECTS =

lib/libgmock_maind.so.1.11.0: googletest-build/googlemock/CMakeFiles/gmock_main.dir/src/gmock_main.cc.o
lib/libgmock_maind.so.1.11.0: googletest-build/googlemock/CMakeFiles/gmock_main.dir/build.make
lib/libgmock_maind.so.1.11.0: lib/libgmockd.so.1.11.0
lib/libgmock_maind.so.1.11.0: lib/libgtestd.so.1.11.0
lib/libgmock_maind.so.1.11.0: googletest-build/googlemock/CMakeFiles/gmock_main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/bustub/cmake-build-debug_wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library ../../lib/libgmock_maind.so"
	cd /mnt/d/bustub/cmake-build-debug_wsl/googletest-build/googlemock && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gmock_main.dir/link.txt --verbose=$(VERBOSE)
	cd /mnt/d/bustub/cmake-build-debug_wsl/googletest-build/googlemock && $(CMAKE_COMMAND) -E cmake_symlink_library ../../lib/libgmock_maind.so.1.11.0 ../../lib/libgmock_maind.so.1.11.0 ../../lib/libgmock_maind.so

lib/libgmock_maind.so: lib/libgmock_maind.so.1.11.0
	@$(CMAKE_COMMAND) -E touch_nocreate lib/libgmock_maind.so

# Rule to build all files generated by this target.
googletest-build/googlemock/CMakeFiles/gmock_main.dir/build: lib/libgmock_maind.so

.PHONY : googletest-build/googlemock/CMakeFiles/gmock_main.dir/build

googletest-build/googlemock/CMakeFiles/gmock_main.dir/clean:
	cd /mnt/d/bustub/cmake-build-debug_wsl/googletest-build/googlemock && $(CMAKE_COMMAND) -P CMakeFiles/gmock_main.dir/cmake_clean.cmake
.PHONY : googletest-build/googlemock/CMakeFiles/gmock_main.dir/clean

googletest-build/googlemock/CMakeFiles/gmock_main.dir/depend:
	cd /mnt/d/bustub/cmake-build-debug_wsl && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/bustub /mnt/d/bustub/cmake-build-debug_wsl/googletest-src/googlemock /mnt/d/bustub/cmake-build-debug_wsl /mnt/d/bustub/cmake-build-debug_wsl/googletest-build/googlemock /mnt/d/bustub/cmake-build-debug_wsl/googletest-build/googlemock/CMakeFiles/gmock_main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : googletest-build/googlemock/CMakeFiles/gmock_main.dir/depend

