#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.50)
AC_INIT(pam-face-authentication, 0.1,[http://code.google.com/p/pam_face_authentication/issues/list])
AC_CONFIG_SRCDIR([pam_face_defines.h])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC


export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig"
# Checks for libraries.

AC_CHECK_LIB([dl], [main])

AC_CHECK_LIB(pam, pam_start, ,AC_MSG_ERROR([PAM library missing]))
AC_CHECK_LIB([pam], [main],,AC_MSG_ERROR(libpam-devel >.99.8 Not Found))
AC_CHECK_LIB([m],[pow])
AC_CHECK_LIB([gslcblas],[cblas_dgemm])
AC_CHECK_LIB([gsl],[gsl_blas_dgemm])
AC_CHECK_LIB([gsl], [main],,AC_MSG_ERROR(GNU Scientific Library >=1.9 Not Found))

PKG_CHECK_MODULES(gsl, gsl >= 1.9)
PKG_CHECK_MODULES(OPENCV, opencv >= 1.0.0)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h stdlib.h string.h unistd.h])
AC_CHECK_HEADERS(dlfcn.h,,AC_MSG_ERROR(You need dlfcn.h for compiling the program))
AC_CHECK_HEADER([security/pam_modules.h], ,[AC_MSG_ERROR([PAM headers missing])])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
