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

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
