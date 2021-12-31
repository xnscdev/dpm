AC_INIT([dpm], [1.0])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_PROG_RANLIB
gl_EARLY

AM_INIT_AUTOMAKE
gl_INIT

AC_ARG_WITH([config-dir],
AS_HELP_STRING([--with-config-dir=DIR], [Directory for configuration files]),
[case "$withval" in
    /* )
	config_dir="$withval"
	;;
    * )
	AC_MSG_ERROR([--with-config-dir requires an absolute path argument])
	;;
esac], [adl_RECURSIVE_EVAL([$sysconfdir/dpm], [config_dir])])

AC_DEFINE_UNQUOTED([REPO_LIST_PATH], ["$config_dir/repo"],
[Path to the repository list file])

PKG_CHECK_MODULES([LIBARCHIVE], [libarchive >= 3.0])
AC_SUBST([LIBARCHIVE_CFLAGS])
AC_SUBST([LIBARCHIVE_LIBS])

PKG_CHECK_MODULES([CURL], [libcurl])
AC_SUBST([CURL_CFLAGS])
AC_SUBST([CURL_LIBS])

AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile])
AC_OUTPUT
