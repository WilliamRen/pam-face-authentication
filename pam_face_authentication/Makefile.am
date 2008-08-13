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
	mkdir -p /lib/pamface
	mkdir -p /lib/pamface/facemanager
	cp *.xml /lib/pamface
	cp faceauthenticate.so /lib/pamface/
	cp pam_face_authenticate.so /lib/security/
	cp libfaceconfigure.so /lib/pamface/
	cp libfacedetect.so /lib/pamface/
	cp gtk-facetracker /lib/pamface/
	cp gtk-facemanager /usr/bin/
	echo>/lib/pamface/xauth.key
	chmod 770 /lib/pamface/*
	chmod 777 /lib/pamface/gtk-facetracker
	chmod 777 /lib/pamface/*.xml
	
clean:
	rm -f *.o *.so gtk-facemanager gtk-facetracker
