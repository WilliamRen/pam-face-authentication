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
char *HAAR_CASCADE_FACE="/lib/pamface/haarcascade.xml";
char *HAAR_CASCADE_EYE="/lib/pamface/haarcascade_eye.xml";
char *HAAR_CASCADE_NOSE="/lib/pamface/haarcascade_nose.xml";


char *FACE_DETECT_LIBRARY="/lib/pamface/libfacedetect.so";
char *FACE_AUTHENTICATE_LIBRARY="/lib/pamface/faceauthenticate.so";
char *FACE_CONFIGURE_LIBRARY="/lib/pamface/libfaceconfigure.so";
char *path="/lib/pamface/";
char *imgPath="/lib/pamface/facemanager/";
char *imgExt=".pgm";
char *GTK_FACE_AUTHENTICATE="/lib/pamface/gtk-facetracker &";
char *XAUTH_EXTRACT_COMMAND="xauth -f /var/lib/gdm/:0.Xauth -b list>/lib/pamface/xauth.key";
char *XAUTH_EXTRACT_FILE="/lib/pamface/xauth.key";
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
