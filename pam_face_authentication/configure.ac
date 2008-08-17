#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.50)
AC_INIT(pam-face-authentication, 0.2,[http://code.google.com/p/pam_face_authentication/issues/list])
AC_CONFIG_SRCDIR([pam_face_defines.h])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

AC_CHECK_LIB(pam, pam_start, ,AC_MSG_ERROR([PAM library missing]))
AC_CHECK_LIB([pam], [main],,AC_MSG_ERROR(libpam-devel >.99.8 Not Found))
AC_CHECK_HEADER([security/pam_modules.h], ,[AC_MSG_ERROR([PAM headers missing])])

PKG_CHECK_MODULES(GSL, gsl >= 1.9)
AC_SUBST(GSL_CFLAGS)
AC_SUBST(GSL_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.10)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(OPENCV, opencv >= 1.0.0)
AC_SUBST(OPENCV_CFLAGS)
AC_SUBST(OPENCV_LIBS)

AC_CONFIG_FILES([
	Makefile
	])
AC_OUTPUT
