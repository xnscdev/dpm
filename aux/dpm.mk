# This Makefile fragment should always be included as the first line of a
# package's Makefile. It defines default blank targets for all steps of the
# DPM package build process and exports variables for the machine target and
# various compilers.

# The following variables can be modified in a package's Makefile to affect
# how a package is built. They should be set after including this file.
#
# CC_TARGET        - The target string to pass to the Apple Clang compiler
# GNU_TRIPLET      - The GNU-style target triplet
# CC               - The C compiler to use
# CXX              - The C++ compiler to use
# OBJC             - The Objective-C compiler to use
# BUILD_TARGET     - Name of the build target used to build the package

CC_TARGET = $(ARCH)-macosx$(MACOS_VERSION)
GNU_TRIPLET = $(ARCH)-apple-darwin$(KERNEL_VERSION)

export CC = xcrun cc -target $(CC_TARGET)
export CXX = xcrun c++ -target $(CC_TARGET)
export OBJC = xcrun cc -target $(CC_TARGET)

.PHONY: configure
configure::

.PHONY: build
build::

.PHONY: clean
clean::

.PHONY: distclean
distclean::

.PHONY: install
install::

.PHONY: postinstall
postinstall::
