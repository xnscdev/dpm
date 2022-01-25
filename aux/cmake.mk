# This Makefile fragment defines targets that are suitable for a typical
# package built with CMake. The following lists the variables that can be
# set to customize the build process:
#
# USE_BUILD         - Whether to do a BUILD build (separate build directory)
# BUILD_DIR         - Name of the build directory, defaults to `build'
# GEN_MAKE          - Use Makefiles as the CMake generator (Ninja by default)
# CONFIGURE_OPTIONS - Additional options to pass to the CMake configuration
#
# These variables should be set before including this file.

BUILD_DIR ?= build
mkbuilddir = mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) &&

ifeq ($(GEN_MAKE),)
genflags = -G Ninja
buildcmd = ninja
else
genflags =
buildcmd = $(MAKE) $(MAKE_BUILD_OPTIONS)
endif

configure::
	$(mkbuilddir) cmake -DCMAKE_BUILD_TYPE=Release \
	    -DCMAKE_OSX_SYSROOT=macosx $(CONFIGURE_OPTIONS) $(genflags) ..

build::
	$(mkbuilddir) $(buildcmd) $(BUILD_TARGET)

clean::
	$(mkbuilddir) $(buildcmd) clean

distclean::
	rm -rf $(BUILD_DIR)

install::
	$(mkbuilddir) $(buildcmd) install
