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
CMAKE_BINARY_DIR = D:\bustub\cmake-build-debug-mingw

# Include any dependencies generated for this target.
include test/CMakeFiles/buffer_pool_manager_test.dir/depend.make
# Include the progress variables for this target.
include test/CMakeFiles/buffer_pool_manager_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/buffer_pool_manager_test.dir/flags.make

test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.obj: test/CMakeFiles/buffer_pool_manager_test.dir/flags.make
test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.obj: test/CMakeFiles/buffer_pool_manager_test.dir/includes_CXX.rsp
test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.obj: ../test/buffer/buffer_pool_manager_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\bustub\cmake-build-debug-mingw\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.obj"
	cd /d D:\bustub\cmake-build-debug-mingw\test && D:\IDE\mingw\mingw8664\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\buffer_pool_manager_test.dir\buffer\buffer_pool_manager_test.cpp.obj -c D:\bustub\test\buffer\buffer_pool_manager_test.cpp

test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.i"
	cd /d D:\bustub\cmake-build-debug-mingw\test && D:\IDE\mingw\mingw8664\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\bustub\test\buffer\buffer_pool_manager_test.cpp > CMakeFiles\buffer_pool_manager_test.dir\buffer\buffer_pool_manager_test.cpp.i

test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.s"
	cd /d D:\bustub\cmake-build-debug-mingw\test && D:\IDE\mingw\mingw8664\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\bustub\test\buffer\buffer_pool_manager_test.cpp -o CMakeFiles\buffer_pool_manager_test.dir\buffer\buffer_pool_manager_test.cpp.s

# Object files for target buffer_pool_manager_test
buffer_pool_manager_test_OBJECTS = \
"CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.obj"

# External object files for target buffer_pool_manager_test
buffer_pool_manager_test_EXTERNAL_OBJECTS =

test/buffer_pool_manager_test.exe: test/CMakeFiles/buffer_pool_manager_test.dir/buffer/buffer_pool_manager_test.cpp.obj
test/buffer_pool_manager_test.exe: test/CMakeFiles/buffer_pool_manager_test.dir/build.make
test/buffer_pool_manager_test.exe: lib/libbustub_shared.dll.a
test/buffer_pool_manager_test.exe: lib/libgmock_maind.dll.a
test/buffer_pool_manager_test.exe: lib/libthirdparty_murmur3.dll.a
test/buffer_pool_manager_test.exe: lib/libgmockd.dll.a
test/buffer_pool_manager_test.exe: lib/libgtestd.dll.a
test/buffer_pool_manager_test.exe: test/CMakeFiles/buffer_pool_manager_test.dir/linklibs.rsp
test/buffer_pool_manager_test.exe: test/CMakeFiles/buffer_pool_manager_test.dir/objects1.rsp
test/buffer_pool_manager_test.exe: test/CMakeFiles/buffer_pool_manager_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\bustub\cmake-build-debug-mingw\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable buffer_pool_manager_test.exe"
	cd /d D:\bustub\cmake-build-debug-mingw\test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\buffer_pool_manager_test.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/buffer_pool_manager_test.dir/build: test/buffer_pool_manager_test.exe
.PHONY : test/CMakeFiles/buffer_pool_manager_test.dir/build

test/CMakeFiles/buffer_pool_manager_test.dir/clean:
	cd /d D:\bustub\cmake-build-debug-mingw\test && $(CMAKE_COMMAND) -P CMakeFiles\buffer_pool_manager_test.dir\cmake_clean.cmake
.PHONY : test/CMakeFiles/buffer_pool_manager_test.dir/clean

test/CMakeFiles/buffer_pool_manager_test.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\bustub D:\bustub\test D:\bustub\cmake-build-debug-mingw D:\bustub\cmake-build-debug-mingw\test D:\bustub\cmake-build-debug-mingw\test\CMakeFiles\buffer_pool_manager_test.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/buffer_pool_manager_test.dir/depend

