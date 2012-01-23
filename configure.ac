#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([HaiGo], [0.1], [clemens.dorner@gmail.com])
AM_INIT_AUTOMAKE([-Wall])
#AM_PATH_CHECK
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
AC_CONFIG_SRCDIR([src/run_program.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h stdbool.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])

AC_OUTPUT
