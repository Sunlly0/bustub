# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.20

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\IDE\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "D:\IDE\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\bustub

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\bustub\cmake-build-debug-mingw-1

# Include any dependencies generated for this target.
include test/CMakeFiles/starter_test.dir/depend.make
# Include the progress variables for this target.
include test/CMakeFiles/starter_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/starter_test.dir/flags.make

test/CMakeFiles/starter_test.dir/primer/starter_test.cpp.obj: test/CMakeFiles/starter_test.dir/flags.make
test/CMakeFiles/starter_test.dir/primer/starter_test.cpp.obj: test/CMakeFiles/starter_test.dir/includes_CXX.rsp
test/CMakeFiles/starter_test.dir/primer/starter_test.cpp.obj: ../test/primer/starter_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\bustub\cmake-build-debug-mingw-1\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/starter_test.dir/primer/starter_test.cpp.obj"
	cd /d D:\bustub\cmake-build-debug-mingw-1\test && D:\IDE\mingw\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\starter_test.dir\primer\starter_test.cpp.obj -c D:\bustub\test\primer\starter_test.cpp

test/CMakeFiles/starter_test.dir/primer/starter_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/starter_test.dir/primer/starter_test.cpp.i"
	cd /d D:\bustub\cmake-build-debug-mingw-1\test && D:\IDE\mingw\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\bustub\test\primer\starter_test.cpp > CMakeFiles\starter_test.dir\primer\starter_test.cpp.i

test/CMakeFiles/starter_test.dir/primer/starter_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/starter_test.dir/primer/starter_test.cpp.s"
	cd /d D:\bustub\cmake-build-debug-mingw-1\test && D:\IDE\mingw\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\bustub\test\primer\starter_test.cpp -o CMakeFiles\starter_test.dir\primer\starter_test.cpp.s

# Object files for target starter_test
starter_test_OBJECTS = \
"CMakeFiles/starter_test.dir/primer/starter_test.cpp.obj"

# External object files for target starter_test
starter_test_EXTERNAL_OBJECTS =

test/starter_test.exe: test/CMakeFiles/starter_test.dir/primer/starter_test.cpp.obj
test/starter_test.exe: test/CMakeFiles/starter_test.dir/build.make
test/starter_test.exe: lib/libbustub_shared.dll.a
test/starter_test.exe: lib/libgmock_maind.dll.a
test/starter_test.exe: lib/libthirdparty_murmur3.dll.a
test/starter_test.exe: lib/libgmockd.dll.a
test/starter_test.exe: lib/libgtestd.dll.a
test/starter_test.exe: test/CMakeFiles/starter_test.dir/linklibs.rsp
test/starter_test.exe: test/CMakeFiles/starter_test.dir/objects1.rsp
test/starter_test.exe: test/CMakeFiles/starter_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\bustub\cmake-build-debug-mingw-1\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable starter_test.exe"
	cd /d D:\bustub\cmake-build-debug-mingw-1\test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\starter_test.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/starter_test.dir/build: test/starter_test.exe
.PHONY : test/CMakeFiles/starter_test.dir/build

test/CMakeFiles/starter_test.dir/clean:
	cd /d D:\bustub\cmake-build-debug-mingw-1\test && $(CMAKE_COMMAND) -P CMakeFiles\starter_test.dir\cmake_clean.cmake
.PHONY : test/CMakeFiles/starter_test.dir/clean

test/CMakeFiles/starter_test.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\bustub D:\bustub\test D:\bustub\cmake-build-debug-mingw-1 D:\bustub\cmake-build-debug-mingw-1\test D:\bustub\cmake-build-debug-mingw-1\test\CMakeFiles\starter_test.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/starter_test.dir/depend

