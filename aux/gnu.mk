ifeq ($(USE_VPATH),)
srcdir_rel = .
MKVPATHDIR =
else
srcdir_rel = ..
VPATH_DIR ?= build
MKVPATHDIR = mkdir -p $(VPATH_DIR) && cd $(VPATH_DIR) &&
endif

configure:
	$(MKVPATHDIR) $(srcdir_rel)/configure --host=$(GNU_TRIPLET) \
	    $(CONFIGURE_OPTIONS)

build:
	$(MKVPATHDIR) $(MAKE) $(BUILD_TARGET)

clean:
	$(MKVPATHDIR) $(MAKE) clean

distclean:
	$(MKVPATHDIR) $(MAKE) distclean

install:
	$(MKVPATHDIR) $(MAKE) install
	for dir in $(DESTDIR)/usr/local/share/info \
	    $(patsubst %,$(DESTDIR)%,$(INFO_DIRS)); do \
	    rm -f $$dir/dir; \
	done

postinstall:
	for dir in /usr/local/share/info $(INFO_DIRS); do \
	    for f in $$dir/*.info; do \
		install-info --dir-file=$$dir/dir $$f; \
	    done; \
	done
