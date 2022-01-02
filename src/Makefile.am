AM_CPPFLAGS = -D_GNU_SOURCE -I$(top_srcdir)/lib -I$(top_builddir)/lib
AM_CFLAGS = -std=gnu11 -Wall -pedantic -Werror=implicit -Wno-switch \
	$(CURL_CFLAGS) $(LIBARCHIVE_CFLAGS)

bin_PROGRAMS = dpm

dpm_SOURCES =		\
	build.c		\
	download.c	\
	download.h	\
	install.c	\
	list.c		\
	list.h		\
	main.c		\
	package.c	\
	package.h	\
	repo.c		\
	repo.h		\
	version.c	\
	version.h

dpm_LDADD = $(top_builddir)/lib/libgnu.a $(LIB_HARD_LOCALE) $(LIB_MBRTOWC) \
	$(LIB_SETLOCALE_NULL) $(LIBINTL) $(CURL_LIBS) $(LIBARCHIVE_LIBS)
