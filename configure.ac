#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.50)
AC_INIT(pam-face-authentication, 0.3,[http://code.google.com/p/pam_face_authentication/issues/list])
AC_CONFIG_SRCDIR([pam_face_defines.h])
AM_INIT_AUTOMAKE([1.10.1])
 
AM_CONFIG_HEADER([config.h])


# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

# Checks for libraries.

AC_CHECK_HEADER(X11/Xlib.h, check_x11="yes",check_x11="no")
  if test x${check_x11} = xno ; then
	AC_MSG_ERROR([X11/Xlib.h Not Found.])

  fi
  if test x${check_x11} = xyes ; then
    AC_CHECK_LIB(X11, XOpenDisplay,
                 [ X11_LIBS="-lX11"
                   AC_DEFINE(USE_X11, 1, [Define to use X11 copy'n'paste]) ],
                 [])
	AC_SUBST(X11_LIBS)

  fi


AC_CHECK_LIB(pam, pam_start, ,AC_MSG_ERROR([PAM library missing]))
AC_CHECK_LIB([pam], [main],,AC_MSG_ERROR(libpam-devel >.99.8 Not Found))
AC_CHECK_HEADER([security/pam_modules.h], ,[AC_MSG_ERROR([PAM headers missing])])



PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.10)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(OPENCV, opencv >= 1.1.0)
AC_SUBST(OPENCV_CFLAGS)
AC_SUBST(OPENCV_LIBS)

AC_CONFIG_FILES([
	Makefile
	])
AC_OUTPUT
