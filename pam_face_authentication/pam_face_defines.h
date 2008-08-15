/*
    Copyright (C) 2008 Rohan Anil (rohan.anil@gmail.com) , Alex Lau ( avengermojo@gmail.com)

    Google Summer of Code Program 2008
    Mentoring Organization: openSUSE
    Mentor: Alex Lau

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/#define IPC_KEY_IMAGE 567814
#define IPC_KEY_STATUS 567813
#define IMAGE_SIZE 230400
#define CANCEL 14
#define AUTHENTICATE 7
#define DISPLAY_ERROR 1
#define EXIT_GUI 2
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240
char *HAAR_CASCADE_FACE="/etc/pam_face_authentication/haarcascade.xml";
char *HAAR_CASCADE_EYE="/etc/pam_face_authentication/haarcascade_eye.xml";
char *HAAR_CASCADE_NOSE="/etc/pam_face_authentication/haarcascade_nose.xml";


char *FACE_DETECT_LIBRARY="/etc/pam_face_authentication/libfacedetect.so";
char *FACE_AUTHENTICATE_LIBRARY="/etc/pam_face_authentication/faceauthenticate.so";
char *FACE_CONFIGURE_LIBRARY="/etc/pam_face_authentication/libfaceconfigure.so";
char *path="/etc/pam_face_authentication/";
char *imgPath="/etc/pam_face_authentication/facemanager/";
char *imgExt=".pgm";
char *GTK_FACE_AUTHENTICATE="/etc/pam_face_authentication/gtk-facetracker &";
char *XAUTH_EXTRACT_COMMAND="xauth -f /var/lib/gdm/:0.Xauth -b list>/etc/pam_face_authentication/xauth.key";
char *XAUTH_EXTRACT_FILE="/etc/pam_face_authentication/xauth.key";
char *XML_GTK_BUILDER_FACE_MANAGER="/etc/pam_face_authentication/gtk-facemanager.xml";
char *XML_GTK_BUILDER_FACE_AUTHENTICATE="/etc/pam_face_authentication/gtk-faceauthenticate.xml";
char *GTK_FACE_MANAGER_KEY="/etc/pam_face_authentication/facemanager/face.key";
static CvScalar colors[] =
    {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}}
    };
