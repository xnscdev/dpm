ifeq ($(USE_VPATH),)
srcdir_rel = .
MKVPATHDIR =
else
srcdir_rel = ..
VPATH_DIR ?= build
MKVPATHDIR = mkdir -p $(VPATH_DIR) && cd $(VPATH_DIR) &&
endif

configure:
	$(MKVPATHDIR) $(srcdir_rel)/configure $(CONFIGURE_OPTIONS)

build:
	$(MKVPATHDIR) $(MAKE) $(BUILD_TARGET)

clean:
	$(MKVPATHDIR) $(MAKE) clean

distclean:
	$(MKVPATHDIR) $(MAKE) distclean

install:
	$(MKVPATHDIR) $(MAKE) install

postinstall:
	for dir in /usr/local/share/info $(INFO_DIRS); \
	    for f in $$dir/*.info; do \
		install-info --dir-file=$$dir/dir $$f; \
	    done; \
	done
