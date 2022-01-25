# This Makefile fragment defines targets that are suitable for a typical
# package built with the Meson build system. The following lists the
# variables that can be set to customize the build process:
#
# BUILD_DIR         - Name of the build directory, defaults to `build'
# CONFIGURE_OPTIONS - Additional options to pass to the Meson configure
#                     invocation
#
# These variables should be set before including this file.

BUILD_DIR ?= build

mkbuilddir = mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) &&

configure::
	$(mkbuilddir) meson --buildtype=release $(CONFIGURE_OPTIONS)

build::
	$(mkbuilddir) ninja

clean::
	$(mkbuilddir) ninja clean

distclean::
	rm -rf $(BUILD_DIR)

install::
	$(mkbuilddir) ninja install

postinstall::
	for dir in /usr/local/share/info $(INFO_DIRS); do \
	    for f in $$dir/*.info; do \
		install-info --dir-file=$$dir/dir $$f; \
	    done; \
	done
