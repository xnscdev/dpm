# This Makefile fragment should always be included as the first line of a
# package's Makefile. It defines default blank targets for all steps of the
# DPM package build process and exports variables for the machine target and
# various compilers.

# Target for the Apple Clang compiler
CC_TARGET = $(ARCH)-macosx$(MACOS_VERSION)

# GNU target triplet for the host machine
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
