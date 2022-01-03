# This Makefile fragment allows for one or more patches to be applied before
# configuration. This file should be included in a package's Makefile before any
# other recipes for the `configure' target are listed. The following variables
# can be set before including this Makefile to adjust how patches are applied:
#
# STRIP_LEVEL - Number of path components to strip (the -p option of patch)

STRIP_LEVEL ?= 1

configure::
	if ! test -f .dpm/Patched; then \
	    for p in `find .dpm -name "*.patch"`; do \
	        patch -Np$(STRIP_LEVEL) -i $$p; \
	    done; \
	    touch .dpm/Patched; \
	fi
