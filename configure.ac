#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(AR_Flight_Controller, 1.1, pp.pele.pierre@gmail.com, AR_Flight_Controller, http://upsilonaudio.com)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([AR_Flight_Controller_config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for SDL library.
PKG_CHECK_MODULES(SDL, sdl)

LIBS="$SDL_LIBS $LIBS"
CFLAGS="$SDL_CFLAGS $CFLAGS"

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero socket])

AC_ARG_ENABLE([nfc],
[AC_HELP_STRING([--enable-nfc],
[Enable NFC Support (Explore NFC / Raspberry Pi)])],
[
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.40])
PKG_CHECK_MODULES([GTHREAD], [gthread-2.0 >= 2.40])
PKG_CHECK_MODULES([GIO], [gio-2.0 >= 2.40])
PKG_CHECK_MODULES([NEARDAL], [neardal])

LIBS="$GLIB_LIBS $GTHREAD_LIBS $GIO_LIBS $NEARDAL_LIBS $LIBS"
CFLAGS="$GLIB_CFLAGS $GTHREAD_CFLAGS $GIO_CFLAGS $GIOUNIX_CFLAGS $NEARDAL_CFLAGS $CFLAGS"
AC_DEFINE([HAVE_NFC_ENABLE],[1],[Should we enable NFC support.])
]
,
[echo "Disable NFC Support"])
AM_CONDITIONAL(NFC_SUPPORT, test "x$enable_nfc" = "xyes")

AC_ARG_WITH([sysroot], 
  [AS_HELP_STRING([--with-sysroot=SYSROOT],
	  [use sysroot as the system root during the build])],
  [if test x"$withval" != x ; then
     SYSROOT_FLAGS="--sysroot=$withval"
   fi],
  [SYSROOT_FLAGS=])
AC_SUBST(SYSROOT_FLAGS)

AC_ARG_WITH([sysconfig], 
  [AS_HELP_STRING([--with-sysconfig=SYSCONFIG],
	  [use sysconfig as the system config dir during the build])],
  [if test x"$withval" != x ; then
     SYSCONFIG="$withval"
   fi],
  [SYSCONFIG=${sysconfdir}])
AC_SUBST(SYSCONFIG)

AC_CONFIG_FILES([
        Makefile
        src/Makefile
        ])

AC_OUTPUT

echo "
AR_Flight_Controller $VERSION
==============================================

Build informations :

Compiler :               ${CC}
CFLAGS :                 ${CFLAGS}
LIBS :                   ${LIBS}
LDFLAGS :                ${LDFLAGS}

System paths :

Prefix :                 ${prefix}"

echo ""
