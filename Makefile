# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.6

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/sda3/darksid3hack0r/projectPFA/branch/qtbranch

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/sda3/darksid3hack0r/projectPFA/branch/qtbranch

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target install
install: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Install the project..."
	/usr/bin/cmake -P cmake_install.cmake
.PHONY : install

# Special rule for the target install
install/fast: preinstall/fast
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Install the project..."
	/usr/bin/cmake -P cmake_install.cmake
.PHONY : install/fast

# Special rule for the target install/local
install/local: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing only the local directory..."
	/usr/bin/cmake -DCMAKE_INSTALL_LOCAL_ONLY=1 -P cmake_install.cmake
.PHONY : install/local

# Special rule for the target install/local
install/local/fast: install/local
.PHONY : install/local/fast

# Special rule for the target install/strip
install/strip: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing the project stripped..."
	/usr/bin/cmake -DCMAKE_INSTALL_DO_STRIP=1 -P cmake_install.cmake
.PHONY : install/strip

# Special rule for the target install/strip
install/strip/fast: install/strip
.PHONY : install/strip/fast

# Special rule for the target list_install_components
list_install_components:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Available install components are: \"Unspecified\""
.PHONY : list_install_components

# Special rule for the target list_install_components
list_install_components/fast: list_install_components
.PHONY : list_install_components/fast

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
	$(CMAKE_COMMAND) -E cmake_progress_start /mnt/sda3/darksid3hack0r/projectPFA/branch/qtbranch/CMakeFiles /mnt/sda3/darksid3hack0r/projectPFA/branch/qtbranch/CMakeFiles/progress.make
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /mnt/sda3/darksid3hack0r/projectPFA/branch/qtbranch/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named pam_face_authenticate

# Build rule for target.
pam_face_authenticate: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 pam_face_authenticate
.PHONY : pam_face_authenticate

# fast build rule for target.
pam_face_authenticate/fast:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/build
.PHONY : pam_face_authenticate/fast

#=============================================================================
# Target rules for targets named qt-faceauth

# Build rule for target.
qt-faceauth: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 qt-faceauth
.PHONY : qt-faceauth

# fast build rule for target.
qt-faceauth/fast:
	$(MAKE) -f CMakeFiles/qt-faceauth.dir/build.make CMakeFiles/qt-faceauth.dir/build
.PHONY : qt-faceauth/fast

#=============================================================================
# Target rules for targets named qt-facemanager

# Build rule for target.
qt-facemanager: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 qt-facemanager
.PHONY : qt-facemanager

# fast build rule for target.
qt-facemanager/fast:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/build
.PHONY : qt-facemanager/fast

#=============================================================================
# Target rules for targets named kgreet_faceauthenticate

# Build rule for target.
kgreet_faceauthenticate: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 kgreet_faceauthenticate
.PHONY : kgreet_faceauthenticate

# fast build rule for target.
kgreet_faceauthenticate/fast:
	$(MAKE) -f kgreet_plugin/CMakeFiles/kgreet_faceauthenticate.dir/build.make kgreet_plugin/CMakeFiles/kgreet_faceauthenticate.dir/build
.PHONY : kgreet_faceauthenticate/fast

#=============================================================================
# Target rules for targets named kgreet_faceauthenticate_automoc

# Build rule for target.
kgreet_faceauthenticate_automoc: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 kgreet_faceauthenticate_automoc
.PHONY : kgreet_faceauthenticate_automoc

# fast build rule for target.
kgreet_faceauthenticate_automoc/fast:
	$(MAKE) -f kgreet_plugin/CMakeFiles/kgreet_faceauthenticate_automoc.dir/build.make kgreet_plugin/CMakeFiles/kgreet_faceauthenticate_automoc.dir/build
.PHONY : kgreet_faceauthenticate_automoc/fast

#=============================================================================
# Target rules for targets named uninstall

# Build rule for target.
uninstall: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 uninstall
.PHONY : uninstall

# fast build rule for target.
uninstall/fast:
	$(MAKE) -f kgreet_plugin/CMakeFiles/uninstall.dir/build.make kgreet_plugin/CMakeFiles/uninstall.dir/build
.PHONY : uninstall/fast

detector.o: detector.cpp.o
.PHONY : detector.o

# target to build an object file
detector.cpp.o:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/detector.cpp.o
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/detector.cpp.o
.PHONY : detector.cpp.o

detector.i: detector.cpp.i
.PHONY : detector.i

# target to preprocess a source file
detector.cpp.i:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/detector.cpp.i
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/detector.cpp.i
.PHONY : detector.cpp.i

detector.s: detector.cpp.s
.PHONY : detector.s

# target to generate assembly for a file
detector.cpp.s:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/detector.cpp.s
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/detector.cpp.s
.PHONY : detector.cpp.s

eyesDetector.o: eyesDetector.cpp.o
.PHONY : eyesDetector.o

# target to build an object file
eyesDetector.cpp.o:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/eyesDetector.cpp.o
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/eyesDetector.cpp.o
.PHONY : eyesDetector.cpp.o

eyesDetector.i: eyesDetector.cpp.i
.PHONY : eyesDetector.i

# target to preprocess a source file
eyesDetector.cpp.i:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/eyesDetector.cpp.i
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/eyesDetector.cpp.i
.PHONY : eyesDetector.cpp.i

eyesDetector.s: eyesDetector.cpp.s
.PHONY : eyesDetector.s

# target to generate assembly for a file
eyesDetector.cpp.s:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/eyesDetector.cpp.s
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/eyesDetector.cpp.s
.PHONY : eyesDetector.cpp.s

faceAuth.o: faceAuth.cpp.o
.PHONY : faceAuth.o

# target to build an object file
faceAuth.cpp.o:
	$(MAKE) -f CMakeFiles/qt-faceauth.dir/build.make CMakeFiles/qt-faceauth.dir/faceAuth.cpp.o
.PHONY : faceAuth.cpp.o

faceAuth.i: faceAuth.cpp.i
.PHONY : faceAuth.i

# target to preprocess a source file
faceAuth.cpp.i:
	$(MAKE) -f CMakeFiles/qt-faceauth.dir/build.make CMakeFiles/qt-faceauth.dir/faceAuth.cpp.i
.PHONY : faceAuth.cpp.i

faceAuth.s: faceAuth.cpp.s
.PHONY : faceAuth.s

# target to generate assembly for a file
faceAuth.cpp.s:
	$(MAKE) -f CMakeFiles/qt-faceauth.dir/build.make CMakeFiles/qt-faceauth.dir/faceAuth.cpp.s
.PHONY : faceAuth.cpp.s

faceDetector.o: faceDetector.cpp.o
.PHONY : faceDetector.o

# target to build an object file
faceDetector.cpp.o:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/faceDetector.cpp.o
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/faceDetector.cpp.o
.PHONY : faceDetector.cpp.o

faceDetector.i: faceDetector.cpp.i
.PHONY : faceDetector.i

# target to preprocess a source file
faceDetector.cpp.i:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/faceDetector.cpp.i
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/faceDetector.cpp.i
.PHONY : faceDetector.cpp.i

faceDetector.s: faceDetector.cpp.s
.PHONY : faceDetector.s

# target to generate assembly for a file
faceDetector.cpp.s:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/faceDetector.cpp.s
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/faceDetector.cpp.s
.PHONY : faceDetector.cpp.s

main.o: main.cpp.o
.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i
.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s
.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/main.cpp.s
.PHONY : main.cpp.s

moc_faceAuth.o: moc_faceAuth.cxx.o
.PHONY : moc_faceAuth.o

# target to build an object file
moc_faceAuth.cxx.o:
	$(MAKE) -f CMakeFiles/qt-faceauth.dir/build.make CMakeFiles/qt-faceauth.dir/moc_faceAuth.cxx.o
.PHONY : moc_faceAuth.cxx.o

moc_faceAuth.i: moc_faceAuth.cxx.i
.PHONY : moc_faceAuth.i

# target to preprocess a source file
moc_faceAuth.cxx.i:
	$(MAKE) -f CMakeFiles/qt-faceauth.dir/build.make CMakeFiles/qt-faceauth.dir/moc_faceAuth.cxx.i
.PHONY : moc_faceAuth.cxx.i

moc_faceAuth.s: moc_faceAuth.cxx.s
.PHONY : moc_faceAuth.s

# target to generate assembly for a file
moc_faceAuth.cxx.s:
	$(MAKE) -f CMakeFiles/qt-faceauth.dir/build.make CMakeFiles/qt-faceauth.dir/moc_faceAuth.cxx.s
.PHONY : moc_faceAuth.cxx.s

moc_faceTrainer.o: moc_faceTrainer.cxx.o
.PHONY : moc_faceTrainer.o

# target to build an object file
moc_faceTrainer.cxx.o:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/moc_faceTrainer.cxx.o
.PHONY : moc_faceTrainer.cxx.o

moc_faceTrainer.i: moc_faceTrainer.cxx.i
.PHONY : moc_faceTrainer.i

# target to preprocess a source file
moc_faceTrainer.cxx.i:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/moc_faceTrainer.cxx.i
.PHONY : moc_faceTrainer.cxx.i

moc_faceTrainer.s: moc_faceTrainer.cxx.s
.PHONY : moc_faceTrainer.s

# target to generate assembly for a file
moc_faceTrainer.cxx.s:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/moc_faceTrainer.cxx.s
.PHONY : moc_faceTrainer.cxx.s

moc_faceTrainerAdvSettings.o: moc_faceTrainerAdvSettings.cxx.o
.PHONY : moc_faceTrainerAdvSettings.o

# target to build an object file
moc_faceTrainerAdvSettings.cxx.o:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/moc_faceTrainerAdvSettings.cxx.o
.PHONY : moc_faceTrainerAdvSettings.cxx.o

moc_faceTrainerAdvSettings.i: moc_faceTrainerAdvSettings.cxx.i
.PHONY : moc_faceTrainerAdvSettings.i

# target to preprocess a source file
moc_faceTrainerAdvSettings.cxx.i:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/moc_faceTrainerAdvSettings.cxx.i
.PHONY : moc_faceTrainerAdvSettings.cxx.i

moc_faceTrainerAdvSettings.s: moc_faceTrainerAdvSettings.cxx.s
.PHONY : moc_faceTrainerAdvSettings.s

# target to generate assembly for a file
moc_faceTrainerAdvSettings.cxx.s:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/moc_faceTrainerAdvSettings.cxx.s
.PHONY : moc_faceTrainerAdvSettings.cxx.s

opencvWebcam.o: opencvWebcam.cpp.o
.PHONY : opencvWebcam.o

# target to build an object file
opencvWebcam.cpp.o:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/opencvWebcam.cpp.o
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/opencvWebcam.cpp.o
.PHONY : opencvWebcam.cpp.o

opencvWebcam.i: opencvWebcam.cpp.i
.PHONY : opencvWebcam.i

# target to preprocess a source file
opencvWebcam.cpp.i:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/opencvWebcam.cpp.i
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/opencvWebcam.cpp.i
.PHONY : opencvWebcam.cpp.i

opencvWebcam.s: opencvWebcam.cpp.s
.PHONY : opencvWebcam.s

# target to generate assembly for a file
opencvWebcam.cpp.s:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/opencvWebcam.cpp.s
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/opencvWebcam.cpp.s
.PHONY : opencvWebcam.cpp.s

pam_face_authentication.o: pam_face_authentication.cpp.o
.PHONY : pam_face_authentication.o

# target to build an object file
pam_face_authentication.cpp.o:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/pam_face_authentication.cpp.o
.PHONY : pam_face_authentication.cpp.o

pam_face_authentication.i: pam_face_authentication.cpp.i
.PHONY : pam_face_authentication.i

# target to preprocess a source file
pam_face_authentication.cpp.i:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/pam_face_authentication.cpp.i
.PHONY : pam_face_authentication.cpp.i

pam_face_authentication.s: pam_face_authentication.cpp.s
.PHONY : pam_face_authentication.s

# target to generate assembly for a file
pam_face_authentication.cpp.s:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/pam_face_authentication.cpp.s
.PHONY : pam_face_authentication.cpp.s

qrc_graphics.o: qrc_graphics.cxx.o
.PHONY : qrc_graphics.o

# target to build an object file
qrc_graphics.cxx.o:
	$(MAKE) -f CMakeFiles/qt-faceauth.dir/build.make CMakeFiles/qt-faceauth.dir/qrc_graphics.cxx.o
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/qrc_graphics.cxx.o
.PHONY : qrc_graphics.cxx.o

qrc_graphics.i: qrc_graphics.cxx.i
.PHONY : qrc_graphics.i

# target to preprocess a source file
qrc_graphics.cxx.i:
	$(MAKE) -f CMakeFiles/qt-faceauth.dir/build.make CMakeFiles/qt-faceauth.dir/qrc_graphics.cxx.i
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/qrc_graphics.cxx.i
.PHONY : qrc_graphics.cxx.i

qrc_graphics.s: qrc_graphics.cxx.s
.PHONY : qrc_graphics.s

# target to generate assembly for a file
qrc_graphics.cxx.s:
	$(MAKE) -f CMakeFiles/qt-faceauth.dir/build.make CMakeFiles/qt-faceauth.dir/qrc_graphics.cxx.s
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/qrc_graphics.cxx.s
.PHONY : qrc_graphics.cxx.s

qtUtils.o: qtUtils.cpp.o
.PHONY : qtUtils.o

# target to build an object file
qtUtils.cpp.o:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/qtUtils.cpp.o
.PHONY : qtUtils.cpp.o

qtUtils.i: qtUtils.cpp.i
.PHONY : qtUtils.i

# target to preprocess a source file
qtUtils.cpp.i:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/qtUtils.cpp.i
.PHONY : qtUtils.cpp.i

qtUtils.s: qtUtils.cpp.s
.PHONY : qtUtils.s

# target to generate assembly for a file
qtUtils.cpp.s:
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/qtUtils.cpp.s
.PHONY : qtUtils.cpp.s

tracker.o: tracker.cpp.o
.PHONY : tracker.o

# target to build an object file
tracker.cpp.o:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/tracker.cpp.o
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/tracker.cpp.o
.PHONY : tracker.cpp.o

tracker.i: tracker.cpp.i
.PHONY : tracker.i

# target to preprocess a source file
tracker.cpp.i:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/tracker.cpp.i
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/tracker.cpp.i
.PHONY : tracker.cpp.i

tracker.s: tracker.cpp.s
.PHONY : tracker.s

# target to generate assembly for a file
tracker.cpp.s:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/tracker.cpp.s
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/tracker.cpp.s
.PHONY : tracker.cpp.s

utils.o: utils.cpp.o
.PHONY : utils.o

# target to build an object file
utils.cpp.o:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/utils.cpp.o
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/utils.cpp.o
.PHONY : utils.cpp.o

utils.i: utils.cpp.i
.PHONY : utils.i

# target to preprocess a source file
utils.cpp.i:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/utils.cpp.i
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/utils.cpp.i
.PHONY : utils.cpp.i

utils.s: utils.cpp.s
.PHONY : utils.s

# target to generate assembly for a file
utils.cpp.s:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/utils.cpp.s
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/utils.cpp.s
.PHONY : utils.cpp.s

verifier.o: verifier.cpp.o
.PHONY : verifier.o

# target to build an object file
verifier.cpp.o:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/verifier.cpp.o
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/verifier.cpp.o
.PHONY : verifier.cpp.o

verifier.i: verifier.cpp.i
.PHONY : verifier.i

# target to preprocess a source file
verifier.cpp.i:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/verifier.cpp.i
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/verifier.cpp.i
.PHONY : verifier.cpp.i

verifier.s: verifier.cpp.s
.PHONY : verifier.s

# target to generate assembly for a file
verifier.cpp.s:
	$(MAKE) -f CMakeFiles/pam_face_authenticate.dir/build.make CMakeFiles/pam_face_authenticate.dir/verifier.cpp.s
	$(MAKE) -f CMakeFiles/qt-facemanager.dir/build.make CMakeFiles/qt-facemanager.dir/verifier.cpp.s
.PHONY : verifier.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... install"
	@echo "... install/local"
	@echo "... install/strip"
	@echo "... list_install_components"
	@echo "... pam_face_authenticate"
	@echo "... qt-faceauth"
	@echo "... qt-facemanager"
	@echo "... rebuild_cache"
	@echo "... kgreet_faceauthenticate"
	@echo "... kgreet_faceauthenticate_automoc"
	@echo "... uninstall"
	@echo "... detector.o"
	@echo "... detector.i"
	@echo "... detector.s"
	@echo "... eyesDetector.o"
	@echo "... eyesDetector.i"
	@echo "... eyesDetector.s"
	@echo "... faceAuth.o"
	@echo "... faceAuth.i"
	@echo "... faceAuth.s"
	@echo "... faceDetector.o"
	@echo "... faceDetector.i"
	@echo "... faceDetector.s"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
	@echo "... moc_faceAuth.o"
	@echo "... moc_faceAuth.i"
	@echo "... moc_faceAuth.s"
	@echo "... moc_faceTrainer.o"
	@echo "... moc_faceTrainer.i"
	@echo "... moc_faceTrainer.s"
	@echo "... moc_faceTrainerAdvSettings.o"
	@echo "... moc_faceTrainerAdvSettings.i"
	@echo "... moc_faceTrainerAdvSettings.s"
	@echo "... opencvWebcam.o"
	@echo "... opencvWebcam.i"
	@echo "... opencvWebcam.s"
	@echo "... pam_face_authentication.o"
	@echo "... pam_face_authentication.i"
	@echo "... pam_face_authentication.s"
	@echo "... qrc_graphics.o"
	@echo "... qrc_graphics.i"
	@echo "... qrc_graphics.s"
	@echo "... qtUtils.o"
	@echo "... qtUtils.i"
	@echo "... qtUtils.s"
	@echo "... tracker.o"
	@echo "... tracker.i"
	@echo "... tracker.s"
	@echo "... utils.o"
	@echo "... utils.i"
	@echo "... utils.s"
	@echo "... verifier.o"
	@echo "... verifier.i"
	@echo "... verifier.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

