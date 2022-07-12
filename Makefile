# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_SOURCE_DIR = /home/cloud1/LLVM_pass_test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cloud1/LLVM_pass_test

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	cd /home/cloud1/LLVM_pass_test && $(CMAKE_COMMAND) -E cmake_progress_start /home/cloud1/LLVM_pass_test/CMakeFiles /home/cloud1/LLVM_pass_test/MyPass/CMakeFiles/progress.marks
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f CMakeFiles/Makefile2 MyPass/all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/cloud1/LLVM_pass_test/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f CMakeFiles/Makefile2 MyPass/clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f CMakeFiles/Makefile2 MyPass/preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f CMakeFiles/Makefile2 MyPass/preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	cd /home/cloud1/LLVM_pass_test && $(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

# Convenience name for target.
MyPass/CMakeFiles/MyPass.dir/rule:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f CMakeFiles/Makefile2 MyPass/CMakeFiles/MyPass.dir/rule
.PHONY : MyPass/CMakeFiles/MyPass.dir/rule

# Convenience name for target.
MyPass: MyPass/CMakeFiles/MyPass.dir/rule

.PHONY : MyPass

# fast build rule for target.
MyPass/fast:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/build
.PHONY : MyPass/fast

BogusControlFlow.o: BogusControlFlow.cpp.o

.PHONY : BogusControlFlow.o

# target to build an object file
BogusControlFlow.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/BogusControlFlow.cpp.o
.PHONY : BogusControlFlow.cpp.o

BogusControlFlow.i: BogusControlFlow.cpp.i

.PHONY : BogusControlFlow.i

# target to preprocess a source file
BogusControlFlow.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/BogusControlFlow.cpp.i
.PHONY : BogusControlFlow.cpp.i

BogusControlFlow.s: BogusControlFlow.cpp.s

.PHONY : BogusControlFlow.s

# target to generate assembly for a file
BogusControlFlow.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/BogusControlFlow.cpp.s
.PHONY : BogusControlFlow.cpp.s

CryptoUtils.o: CryptoUtils.cpp.o

.PHONY : CryptoUtils.o

# target to build an object file
CryptoUtils.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/CryptoUtils.cpp.o
.PHONY : CryptoUtils.cpp.o

CryptoUtils.i: CryptoUtils.cpp.i

.PHONY : CryptoUtils.i

# target to preprocess a source file
CryptoUtils.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/CryptoUtils.cpp.i
.PHONY : CryptoUtils.cpp.i

CryptoUtils.s: CryptoUtils.cpp.s

.PHONY : CryptoUtils.s

# target to generate assembly for a file
CryptoUtils.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/CryptoUtils.cpp.s
.PHONY : CryptoUtils.cpp.s

Flattening.o: Flattening.cpp.o

.PHONY : Flattening.o

# target to build an object file
Flattening.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/Flattening.cpp.o
.PHONY : Flattening.cpp.o

Flattening.i: Flattening.cpp.i

.PHONY : Flattening.i

# target to preprocess a source file
Flattening.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/Flattening.cpp.i
.PHONY : Flattening.cpp.i

Flattening.s: Flattening.cpp.s

.PHONY : Flattening.s

# target to generate assembly for a file
Flattening.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/Flattening.cpp.s
.PHONY : Flattening.cpp.s

Utils.o: Utils.cpp.o

.PHONY : Utils.o

# target to build an object file
Utils.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/Utils.cpp.o
.PHONY : Utils.cpp.o

Utils.i: Utils.cpp.i

.PHONY : Utils.i

# target to preprocess a source file
Utils.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/Utils.cpp.i
.PHONY : Utils.cpp.i

Utils.s: Utils.cpp.s

.PHONY : Utils.s

# target to generate assembly for a file
Utils.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/Utils.cpp.s
.PHONY : Utils.cpp.s

change_func_name.o: change_func_name.cpp.o

.PHONY : change_func_name.o

# target to build an object file
change_func_name.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/change_func_name.cpp.o
.PHONY : change_func_name.cpp.o

change_func_name.i: change_func_name.cpp.i

.PHONY : change_func_name.i

# target to preprocess a source file
change_func_name.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/change_func_name.cpp.i
.PHONY : change_func_name.cpp.i

change_func_name.s: change_func_name.cpp.s

.PHONY : change_func_name.s

# target to generate assembly for a file
change_func_name.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/change_func_name.cpp.s
.PHONY : change_func_name.cpp.s

dom_ana.o: dom_ana.cpp.o

.PHONY : dom_ana.o

# target to build an object file
dom_ana.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/dom_ana.cpp.o
.PHONY : dom_ana.cpp.o

dom_ana.i: dom_ana.cpp.i

.PHONY : dom_ana.i

# target to preprocess a source file
dom_ana.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/dom_ana.cpp.i
.PHONY : dom_ana.cpp.i

dom_ana.s: dom_ana.cpp.s

.PHONY : dom_ana.s

# target to generate assembly for a file
dom_ana.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/dom_ana.cpp.s
.PHONY : dom_ana.cpp.s

insert_basic_block.o: insert_basic_block.cpp.o

.PHONY : insert_basic_block.o

# target to build an object file
insert_basic_block.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/insert_basic_block.cpp.o
.PHONY : insert_basic_block.cpp.o

insert_basic_block.i: insert_basic_block.cpp.i

.PHONY : insert_basic_block.i

# target to preprocess a source file
insert_basic_block.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/insert_basic_block.cpp.i
.PHONY : insert_basic_block.cpp.i

insert_basic_block.s: insert_basic_block.cpp.s

.PHONY : insert_basic_block.s

# target to generate assembly for a file
insert_basic_block.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/insert_basic_block.cpp.s
.PHONY : insert_basic_block.cpp.s

make_func.o: make_func.cpp.o

.PHONY : make_func.o

# target to build an object file
make_func.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/make_func.cpp.o
.PHONY : make_func.cpp.o

make_func.i: make_func.cpp.i

.PHONY : make_func.i

# target to preprocess a source file
make_func.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/make_func.cpp.i
.PHONY : make_func.cpp.i

make_func.s: make_func.cpp.s

.PHONY : make_func.s

# target to generate assembly for a file
make_func.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/make_func.cpp.s
.PHONY : make_func.cpp.s

print_suc_and_next_node.o: print_suc_and_next_node.cpp.o

.PHONY : print_suc_and_next_node.o

# target to build an object file
print_suc_and_next_node.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/print_suc_and_next_node.cpp.o
.PHONY : print_suc_and_next_node.cpp.o

print_suc_and_next_node.i: print_suc_and_next_node.cpp.i

.PHONY : print_suc_and_next_node.i

# target to preprocess a source file
print_suc_and_next_node.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/print_suc_and_next_node.cpp.i
.PHONY : print_suc_and_next_node.cpp.i

print_suc_and_next_node.s: print_suc_and_next_node.cpp.s

.PHONY : print_suc_and_next_node.s

# target to generate assembly for a file
print_suc_and_next_node.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/print_suc_and_next_node.cpp.s
.PHONY : print_suc_and_next_node.cpp.s

slice_on_func_param.o: slice_on_func_param.cpp.o

.PHONY : slice_on_func_param.o

# target to build an object file
slice_on_func_param.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/slice_on_func_param.cpp.o
.PHONY : slice_on_func_param.cpp.o

slice_on_func_param.i: slice_on_func_param.cpp.i

.PHONY : slice_on_func_param.i

# target to preprocess a source file
slice_on_func_param.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/slice_on_func_param.cpp.i
.PHONY : slice_on_func_param.cpp.i

slice_on_func_param.s: slice_on_func_param.cpp.s

.PHONY : slice_on_func_param.s

# target to generate assembly for a file
slice_on_func_param.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/slice_on_func_param.cpp.s
.PHONY : slice_on_func_param.cpp.s

split_basic_block.o: split_basic_block.cpp.o

.PHONY : split_basic_block.o

# target to build an object file
split_basic_block.cpp.o:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/split_basic_block.cpp.o
.PHONY : split_basic_block.cpp.o

split_basic_block.i: split_basic_block.cpp.i

.PHONY : split_basic_block.i

# target to preprocess a source file
split_basic_block.cpp.i:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/split_basic_block.cpp.i
.PHONY : split_basic_block.cpp.i

split_basic_block.s: split_basic_block.cpp.s

.PHONY : split_basic_block.s

# target to generate assembly for a file
split_basic_block.cpp.s:
	cd /home/cloud1/LLVM_pass_test && $(MAKE) -f MyPass/CMakeFiles/MyPass.dir/build.make MyPass/CMakeFiles/MyPass.dir/split_basic_block.cpp.s
.PHONY : split_basic_block.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... MyPass"
	@echo "... BogusControlFlow.o"
	@echo "... BogusControlFlow.i"
	@echo "... BogusControlFlow.s"
	@echo "... CryptoUtils.o"
	@echo "... CryptoUtils.i"
	@echo "... CryptoUtils.s"
	@echo "... Flattening.o"
	@echo "... Flattening.i"
	@echo "... Flattening.s"
	@echo "... Utils.o"
	@echo "... Utils.i"
	@echo "... Utils.s"
	@echo "... change_func_name.o"
	@echo "... change_func_name.i"
	@echo "... change_func_name.s"
	@echo "... dom_ana.o"
	@echo "... dom_ana.i"
	@echo "... dom_ana.s"
	@echo "... insert_basic_block.o"
	@echo "... insert_basic_block.i"
	@echo "... insert_basic_block.s"
	@echo "... make_func.o"
	@echo "... make_func.i"
	@echo "... make_func.s"
	@echo "... print_suc_and_next_node.o"
	@echo "... print_suc_and_next_node.i"
	@echo "... print_suc_and_next_node.s"
	@echo "... slice_on_func_param.o"
	@echo "... slice_on_func_param.i"
	@echo "... slice_on_func_param.s"
	@echo "... split_basic_block.o"
	@echo "... split_basic_block.i"
	@echo "... split_basic_block.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	cd /home/cloud1/LLVM_pass_test && $(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

