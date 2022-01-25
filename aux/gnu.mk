# This Makefile fragment defines targets that are suitable for a typical
# package built with GNU autoconf, automake, etc. The following lists the
# variables that can be set to customize the build process:
#
# USE_VPATH         - Whether to do a VPATH build (separate build directory)
# VPATH_DIR         - Name of the build directory when doing a VPATH build,
#                     defaults to `build'
# CONFIGURE_ENV     - Environment variables to pass to the `./configure` script
# CONFIGURE_OPTIONS - Additional options to pass to the `./configure' script
#
# These variables should be set before including this file.

ifeq ($(USE_VPATH),)
srcdir_rel = .
mkvpathdir =
clean_fail = -
else
srcdir_rel = ..
VPATH_DIR ?= build
mkvpathdir = mkdir -p $(VPATH_DIR) && cd $(VPATH_DIR) &&
clean_fail =
endif

configure::
	$(mkvpathdir) $(CONFIGURE_ENV) $(srcdir_rel)/configure \
	    --build=$(GNU_TRIPLET) $(CONFIGURE_OPTIONS)

build::
	$(mkvpathdir) $(MAKE) $(GNU_BUILD_OPTIONS) $(BUILD_TARGET)

clean::
	$(mkvpathdir) $(MAKE) $(GNU_CLEAN_OPTIONS) clean

distclean::
	$(clean_fail)$(mkvpathdir) $(MAKE) $(GNU_DISTCLEAN_OPTIONS) distclean
ifneq ($(USE_VPATH),)
	rm -rf $(VPATH_DIR)
endif

install::
	$(mkvpathdir) $(MAKE) $(GNU_INSTALL_OPTIONS) install
	for dir in $(DESTDIR)/usr/local/share/info \
	    $(patsubst %,$(DESTDIR)%,$(INFO_DIRS)); do \
	    rm -f $$dir/dir; \
	done

postinstall::
	shopt -s nullglob; \
	for dir in /usr/local/share/info $(INFO_DIRS); do \
	    for f in $$dir/*.info; do \
		install-info --dir-file=$$dir/dir $$f; \
	    done; \
	done
