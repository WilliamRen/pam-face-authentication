
INCLUDES = \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	-DBINDIR=\"$(bindir)\" \
	$(GTK_CFLAGS) \
	$(X11_CFLAGS) \
	$(GSL_CFLAGS) \
	$(OPENCV_CFLAGS)

bin_PROGRAMS = gtk-facemanager gtk-facetracker


gtk_facemanager_SOURCES = gtk-facemanager.c libfaceconfigure.c
gtk_facemanager_LDADD = \
	$(GTK_LIBS) \
	$(GSL_LIBS) \
	$(OPENCV_LIBS) \
	libfacedetect.la
gtk_facemanager_LDFLAGS = -export-dynamic
gtk_facetracker_SOURCES = gtk-faceauthenticate.c
gtk_facetracker_LDADD = \
	$(GTK_LIBS) \
	$(GSL_LIBS) \
	$(OPENCV_LIBS) \
	libfacedetect.la
gtk_facetracker_LDFLAGS = -export-dynamic
extradir=$(prefix)/lib/
lib_LTLIBRARIES = libfaceauthenticate.la
extra_LTLIBRARIES = libfacedetect.la

pammod_PROGRAMS = pam_face_authenticate.so
pammoddir=/lib/security

pam_face_authenticate_so_SOURCES = \
	pam_face_authentication.c \
	pam_face.h \
	pam_face_defines.h

pam_face_authenticate_so_CFLAGS = -L/usr/X11R6/lib -lX11 -fPIC
pam_face_authenticate_so_LDFLAGS = -shared 
pam_face_authenticate_so_LDADD = \
                        $(PAM_LIBS) \
                        $(GSL_LIBS) \
                        $(OPENCV_LIBS) \
		           $(X11_LIBS) \
			libfacedetect.la \
                        libfaceauthenticate.la

extra1dir=/etc/pam-face-authentication/
extra2dir=/etc/pam-face-authentication/facemanager/

extra1_DATA = xauth.key display.key
extra2_DATA = face.key
uidir=$(pkgdatadir)
ui_DATA = \
gtk-faceauthenticate.xml \
gtk-facemanager.xml \
haarcascade_eye.xml \
haarcascade_nose.xml \
haarcascade.xml

EXTRA_DIST=$(ui_DATA) $(extra1_DATA) $(extra2_DATA)
