CC	= gcc
CFLAGS  =
BIN_DIR = 
INC_DIR	= 
LIB_DIR	= 
INCL	= `pkg-config --cflags opencv`			\
	  `pkg-config --cflags gsl`
LIBS	=  `pkg-config --libs opencv`		\
	   `pkg-config --libs gsl`
EXTRA   = -export-dynamic `pkg-config --cflags --libs gtk+-2.0`
EXTRA2  = -lpam -ldl
FILES     = gtk-facemanager  gtk-facetracker libfacedetect.so libfaceconfigure.so pam_face_authenticate.so faceauthenticate.so

all: $(FILES)

libfaceconfigure.so: libfaceconfigure.c
	$(CC) $(CFLAGS) $(INCL) $(LIBS) -shared -o libfaceconfigure.so libfaceconfigure.c 

libfacedetect.so: libfacedetect.c
	$(CC) $(CFLAGS) $(INCL) $(LIBS) -shared -o libfacedetect.so libfacedetect.c 

faceauthenticate.so: faceauthenticate.c
	$(CC) $(CFLAGS) $(INCL) $(LIBS) -shared -o faceauthenticate.so faceauthenticate.c 

pam_face_authenticate.so: pam_face_authentication.c
	$(CC) $(CFLAGS) $(INCL) $(LIBS) $(EXTRA2) -shared -o pam_face_authenticate.so pam_face_authentication.c 

gtk-facemanager: gtk-facemanager.c
	$(CC) $(CFLAGS) $(INCL) $(LIBS) -o $@ gtk-facemanager.c $(EXTRA)

gtk-facetracker: gtk-faceauthenticate.c
	$(CC) $(CFLAGS) $(INCL) $(LIBS) -o $@ gtk-faceauthenticate.c $(EXTRA)
install:
	mkdir -p /etc/pam_face_authentication
	mkdir -p /etc/pam_face_authentication/facemanager
	cp *.xml /etc/pam_face_authentication
	cp faceauthenticate.so /etc/pam_face_authentication/
	cp pam_face_authenticate.so /lib/security/
	cp libfaceconfigure.so /etc/pam_face_authentication/
	cp libfacedetect.so /etc/pam_face_authentication/
	cp gtk-facetracker /etc/pam_face_authentication/
	cp gtk-facemanager /usr/bin/
	echo>/etc/pam_face_authentication/xauth.key
	chmod 770 /etc/pam_face_authentication/*
	chmod 777 /etc/pam_face_authentication/gtk-facetracker
	chmod 777 /etc/pam_face_authentication/*.xml
	
clean:
	rm -f *.o *.so gtk-facemanager gtk-facetracker
