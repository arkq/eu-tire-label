# EU-tire-label - configure.ac
# Copyright (c) 2015 Arkadiusz Bokowy

AC_INIT([EU-tire-label], [0.1.0], [arkadiusz.bokowy@gmail.com])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_PROG_CC

# support for Common Gateway Interface
AC_ARG_ENABLE(
	[cgi],
	[AS_HELP_STRING([--enable-cgi], [enable CGI support])],
)
AM_CONDITIONAL([ENABLE_CGI], [test "x$enable_cgi" = "xyes"])
AM_COND_IF(
	[ENABLE_CGI],
	[AC_DEFINE([ENABLE_CGI], [1], [Define to 1 if the CGI is enabled])],
)

# support for SVG rasterisation (librsvg + cairo)
AC_ARG_ENABLE(
	[rsvg],
	[AS_HELP_STRING([--enable-rsvg], [enable rasterisation support])],
)
AM_CONDITIONAL([ENABLE_RSVG], [test "x$enable_rsvg" = "xyes"])
AM_COND_IF(
	[ENABLE_RSVG],
	[PKG_CHECK_MODULES(
		[LIBRSVG], [librsvg-2.0],
		[AC_DEFINE([ENABLE_RSVG], [1], [Define to 1 if librsvg is enabled])],
		[AC_MSG_ERROR([module librsvg-2.0 not found])],
	)],
)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
