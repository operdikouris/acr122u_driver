# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_SOURCE_DIR = /home/orfeas/acr122u_driver

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/orfeas/acr122u_driver/build

# Include any dependencies generated for this target.
include src/CMakeFiles/acr122u.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/acr122u.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/acr122u.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/acr122u.dir/flags.make

src/CMakeFiles/acr122u.dir/ccid.c.o: src/CMakeFiles/acr122u.dir/flags.make
src/CMakeFiles/acr122u.dir/ccid.c.o: /home/orfeas/acr122u_driver/src/ccid.c
src/CMakeFiles/acr122u.dir/ccid.c.o: src/CMakeFiles/acr122u.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/orfeas/acr122u_driver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/CMakeFiles/acr122u.dir/ccid.c.o"
	cd /home/orfeas/acr122u_driver/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/CMakeFiles/acr122u.dir/ccid.c.o -MF CMakeFiles/acr122u.dir/ccid.c.o.d -o CMakeFiles/acr122u.dir/ccid.c.o -c /home/orfeas/acr122u_driver/src/ccid.c

src/CMakeFiles/acr122u.dir/ccid.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/acr122u.dir/ccid.c.i"
	cd /home/orfeas/acr122u_driver/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/orfeas/acr122u_driver/src/ccid.c > CMakeFiles/acr122u.dir/ccid.c.i

src/CMakeFiles/acr122u.dir/ccid.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/acr122u.dir/ccid.c.s"
	cd /home/orfeas/acr122u_driver/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/orfeas/acr122u_driver/src/ccid.c -o CMakeFiles/acr122u.dir/ccid.c.s

src/CMakeFiles/acr122u.dir/apdu.c.o: src/CMakeFiles/acr122u.dir/flags.make
src/CMakeFiles/acr122u.dir/apdu.c.o: /home/orfeas/acr122u_driver/src/apdu.c
src/CMakeFiles/acr122u.dir/apdu.c.o: src/CMakeFiles/acr122u.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/orfeas/acr122u_driver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object src/CMakeFiles/acr122u.dir/apdu.c.o"
	cd /home/orfeas/acr122u_driver/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/CMakeFiles/acr122u.dir/apdu.c.o -MF CMakeFiles/acr122u.dir/apdu.c.o.d -o CMakeFiles/acr122u.dir/apdu.c.o -c /home/orfeas/acr122u_driver/src/apdu.c

src/CMakeFiles/acr122u.dir/apdu.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/acr122u.dir/apdu.c.i"
	cd /home/orfeas/acr122u_driver/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/orfeas/acr122u_driver/src/apdu.c > CMakeFiles/acr122u.dir/apdu.c.i

src/CMakeFiles/acr122u.dir/apdu.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/acr122u.dir/apdu.c.s"
	cd /home/orfeas/acr122u_driver/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/orfeas/acr122u_driver/src/apdu.c -o CMakeFiles/acr122u.dir/apdu.c.s

src/CMakeFiles/acr122u.dir/acr122u_usb.c.o: src/CMakeFiles/acr122u.dir/flags.make
src/CMakeFiles/acr122u.dir/acr122u_usb.c.o: /home/orfeas/acr122u_driver/src/acr122u_usb.c
src/CMakeFiles/acr122u.dir/acr122u_usb.c.o: src/CMakeFiles/acr122u.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/orfeas/acr122u_driver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object src/CMakeFiles/acr122u.dir/acr122u_usb.c.o"
	cd /home/orfeas/acr122u_driver/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/CMakeFiles/acr122u.dir/acr122u_usb.c.o -MF CMakeFiles/acr122u.dir/acr122u_usb.c.o.d -o CMakeFiles/acr122u.dir/acr122u_usb.c.o -c /home/orfeas/acr122u_driver/src/acr122u_usb.c

src/CMakeFiles/acr122u.dir/acr122u_usb.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/acr122u.dir/acr122u_usb.c.i"
	cd /home/orfeas/acr122u_driver/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/orfeas/acr122u_driver/src/acr122u_usb.c > CMakeFiles/acr122u.dir/acr122u_usb.c.i

src/CMakeFiles/acr122u.dir/acr122u_usb.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/acr122u.dir/acr122u_usb.c.s"
	cd /home/orfeas/acr122u_driver/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/orfeas/acr122u_driver/src/acr122u_usb.c -o CMakeFiles/acr122u.dir/acr122u_usb.c.s

# Object files for target acr122u
acr122u_OBJECTS = \
"CMakeFiles/acr122u.dir/ccid.c.o" \
"CMakeFiles/acr122u.dir/apdu.c.o" \
"CMakeFiles/acr122u.dir/acr122u_usb.c.o"

# External object files for target acr122u
acr122u_EXTERNAL_OBJECTS =

src/libacr122u.a: src/CMakeFiles/acr122u.dir/ccid.c.o
src/libacr122u.a: src/CMakeFiles/acr122u.dir/apdu.c.o
src/libacr122u.a: src/CMakeFiles/acr122u.dir/acr122u_usb.c.o
src/libacr122u.a: src/CMakeFiles/acr122u.dir/build.make
src/libacr122u.a: src/CMakeFiles/acr122u.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/orfeas/acr122u_driver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C static library libacr122u.a"
	cd /home/orfeas/acr122u_driver/build/src && $(CMAKE_COMMAND) -P CMakeFiles/acr122u.dir/cmake_clean_target.cmake
	cd /home/orfeas/acr122u_driver/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/acr122u.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/acr122u.dir/build: src/libacr122u.a
.PHONY : src/CMakeFiles/acr122u.dir/build

src/CMakeFiles/acr122u.dir/clean:
	cd /home/orfeas/acr122u_driver/build/src && $(CMAKE_COMMAND) -P CMakeFiles/acr122u.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/acr122u.dir/clean

src/CMakeFiles/acr122u.dir/depend:
	cd /home/orfeas/acr122u_driver/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/orfeas/acr122u_driver /home/orfeas/acr122u_driver/src /home/orfeas/acr122u_driver/build /home/orfeas/acr122u_driver/build/src /home/orfeas/acr122u_driver/build/src/CMakeFiles/acr122u.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : src/CMakeFiles/acr122u.dir/depend

