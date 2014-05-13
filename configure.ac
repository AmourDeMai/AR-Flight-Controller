AC_PREREQ([2.69])
AC_INIT(AR_Flight_Controller, 1.0, pp.pele.pierre@gmail.com)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([src/AR_Flight_Controller.c])
AM_INIT_AUTOMAKE

# Checks for programs.

AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for libraries.

SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,:,AC_MSG_ERROR([*** SDL version $SDL_VERSION is not installed on your system ! ***]))

# Checks for header files.

AC_CHECK_HEADERS([arpa/inet.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero socket])
AC_CONFIG_FILES([
        Makefile
        src/Makefile
        ])
AC_OUTPUT
