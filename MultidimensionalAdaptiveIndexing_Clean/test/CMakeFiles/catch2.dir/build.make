# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean

# Utility rule file for catch2.

# Include any custom commands dependencies for this target.
include test/CMakeFiles/catch2.dir/compiler_depend.make

# Include the progress variables for this target.
include test/CMakeFiles/catch2.dir/progress.make

test/CMakeFiles/catch2: test/CMakeFiles/catch2-complete

test/CMakeFiles/catch2-complete: test/catch2-prefix/src/catch2-stamp/catch2-install
test/CMakeFiles/catch2-complete: test/catch2-prefix/src/catch2-stamp/catch2-mkdir
test/CMakeFiles/catch2-complete: test/catch2-prefix/src/catch2-stamp/catch2-download
test/CMakeFiles/catch2-complete: test/catch2-prefix/src/catch2-stamp/catch2-update
test/CMakeFiles/catch2-complete: test/catch2-prefix/src/catch2-stamp/catch2-patch
test/CMakeFiles/catch2-complete: test/catch2-prefix/src/catch2-stamp/catch2-configure
test/CMakeFiles/catch2-complete: test/catch2-prefix/src/catch2-stamp/catch2-build
test/CMakeFiles/catch2-complete: test/catch2-prefix/src/catch2-stamp/catch2-install
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Completed 'catch2'"
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E make_directory /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/CMakeFiles
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E touch /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/CMakeFiles/catch2-complete
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E touch /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/catch2-done

test/catch2-prefix/src/catch2-stamp/catch2-build: test/catch2-prefix/src/catch2-stamp/catch2-configure
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Performing build step for 'catch2'"
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-build && $(MAKE)
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-build && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E touch /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/catch2-build

test/catch2-prefix/src/catch2-stamp/catch2-configure: test/catch2-prefix/tmp/catch2-cfgcmd.txt
test/catch2-prefix/src/catch2-stamp/catch2-configure: test/catch2-prefix/src/catch2-stamp/catch2-patch
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Performing configure step for 'catch2'"
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-build && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -DCATCH_BUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/Library/Developer/CommandLineTools/usr/bin/c++ -DCMAKE_C_COMPILER=/opt/homebrew/bin/gcc-11 -DCMAKE_INSTALL_PREFIX=/Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/deps "-GUnix Makefiles" -S /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2 -B /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-build
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-build && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E touch /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/catch2-configure

test/catch2-prefix/src/catch2-stamp/catch2-download: test/catch2-prefix/src/catch2-stamp/download-catch2.cmake
test/catch2-prefix/src/catch2-stamp/catch2-download: test/catch2-prefix/src/catch2-stamp/catch2-urlinfo.txt
test/catch2-prefix/src/catch2-stamp/catch2-download: test/catch2-prefix/src/catch2-stamp/catch2-mkdir
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Performing download step (download, verify and extract) for 'catch2'"
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -DCMAKE_MESSAGE_LOG_LEVEL=VERBOSE -P /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/download-catch2.cmake
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -DCMAKE_MESSAGE_LOG_LEVEL=VERBOSE -P /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/verify-catch2.cmake
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -DCMAKE_MESSAGE_LOG_LEVEL=VERBOSE -P /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/extract-catch2.cmake
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E touch /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/catch2-download

test/catch2-prefix/src/catch2-stamp/catch2-install: test/catch2-prefix/src/catch2-stamp/catch2-build
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Performing install step for 'catch2'"
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-build && $(MAKE) install
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-build && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E touch /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/catch2-install

test/catch2-prefix/src/catch2-stamp/catch2-mkdir:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Creating directories for 'catch2'"
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -Dcfgdir= -P /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/tmp/catch2-mkdirs.cmake
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E touch /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/catch2-mkdir

test/catch2-prefix/src/catch2-stamp/catch2-patch: test/catch2-prefix/src/catch2-stamp/catch2-patch-info.txt
test/catch2-prefix/src/catch2-stamp/catch2-patch: test/catch2-prefix/src/catch2-stamp/catch2-update
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "No patch step for 'catch2'"
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E echo_append
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E touch /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/catch2-patch

test/catch2-prefix/src/catch2-stamp/catch2-update: test/catch2-prefix/src/catch2-stamp/catch2-update-info.txt
test/catch2-prefix/src/catch2-stamp/catch2-update: test/catch2-prefix/src/catch2-stamp/catch2-download
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "No update step for 'catch2'"
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E echo_append
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test && /opt/homebrew/Cellar/cmake/3.30.5/bin/cmake -E touch /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/catch2-prefix/src/catch2-stamp/catch2-update

catch2: test/CMakeFiles/catch2
catch2: test/CMakeFiles/catch2-complete
catch2: test/catch2-prefix/src/catch2-stamp/catch2-build
catch2: test/catch2-prefix/src/catch2-stamp/catch2-configure
catch2: test/catch2-prefix/src/catch2-stamp/catch2-download
catch2: test/catch2-prefix/src/catch2-stamp/catch2-install
catch2: test/catch2-prefix/src/catch2-stamp/catch2-mkdir
catch2: test/catch2-prefix/src/catch2-stamp/catch2-patch
catch2: test/catch2-prefix/src/catch2-stamp/catch2-update
catch2: test/CMakeFiles/catch2.dir/build.make
.PHONY : catch2

# Rule to build all files generated by this target.
test/CMakeFiles/catch2.dir/build: catch2
.PHONY : test/CMakeFiles/catch2.dir/build

test/CMakeFiles/catch2.dir/clean:
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test && $(CMAKE_COMMAND) -P CMakeFiles/catch2.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/catch2.dir/clean

test/CMakeFiles/catch2.dir/depend:
	cd /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test /Users/dinosl/Downloads/MultidimensionalAdaptiveIndexing_Clean/test/CMakeFiles/catch2.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : test/CMakeFiles/catch2.dir/depend

