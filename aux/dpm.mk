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
