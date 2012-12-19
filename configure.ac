#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([libnss_redis], [0.01], [hiroyan@gmail.com])
AC_CONFIG_SRCDIR([src/nss_redis.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

#
# configure libhiredis
#
have_hiredis="1"
AC_CHECK_HEADERS([hiredis/hiredis.h], , [have_hiredis=0])
AC_CHECK_LIB([hiredis], [redisConnect], , [have_hiredis=0])
if test "x$have_hiredis" = "x0" ; then
  AC_MSG_ERROR("cannot find library for -lhiredis.")
fi

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset strdup])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
