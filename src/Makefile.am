AM_CPPFLAGS = -D_GNU_SOURCE -I$(top_srcdir)/lib -I$(top_builddir)/lib
AM_CFLAGS = -std=gnu11 -Wall -pedantic -Werror=implicit

bin_PROGRAMS = dpm

dpm_SOURCES =		\
	main.c		\
	package.c	\
	package.h	\
	repo.c		\
	repo.h

dpm_LDADD = $(top_builddir)/lib/libgnu.a \
	$(LIB_HARD_LOCALE) $(LIB_MBRTOWC) $(LIB_SETLOCALE_NULL) $(LIBINTL)
