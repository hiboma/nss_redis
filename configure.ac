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

CFLAGS="$CFLAGS -std=gnu99 -Wextra -Wall -W -Wno-sign-compare"

#
# configure libhiredis
#
AC_ARG_WITH(hiredis,
        [  --with-hiredis=DIR            Location of your libhiredis installation])

if test -n "$with_hiredis"
then
    HIREDIS_LIB_DIR="$with_hiredis/lib/"
    HIREDIS_INC_DIR="$with_hiredis/include/"
    LDFLAGS="-L$HIREDIS_LIB_DIR $LDFLAGS"
    CPPFLAGS="-I$HIREDIS_INC_DIR $CPPFLAGS"
    AC_MSG_RESULT([$LDFLAGS])
    AC_MSG_RESULT([$CPPFLAGS])
fi

have_hiredis="1"
AC_CHECK_HEADERS([hiredis/hiredis.h], , [have_hiredis=0])
AC_CHECK_LIB([hiredis], [redisConnect], , [have_hiredis=0])
if test "x$have_hiredis" = "x0" ; then
  AC_MSG_ERROR("cannot find library for -lhiredis.")
fi

AC_ARG_ENABLE(debug,
        [  --enable-debug turn on        Enable debug],
        [AC_DEFINE(DEBUG, 1, [Enable debug])])

AC_CHECK_HEADERS([stdlib.h string.h pthread.h nss.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset strncpy])

AC_CONFIG_FILES([Makefile src/Makefile t/Makefile])
AC_OUTPUT
