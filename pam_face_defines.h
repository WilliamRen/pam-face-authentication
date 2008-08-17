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
*/

#define IPC_KEY_IMAGE 567814
#define IPC_KEY_STATUS 567813
#define IMAGE_SIZE 230400
#define CANCEL 14
#define AUTHENTICATE 7
#define DISPLAY_ERROR 1
#define EXIT_GUI 2
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240

extern char *HAAR_CASCADE_FACE;
extern char *HAAR_CASCADE_EYE;
extern char *HAAR_CASCADE_NOSE;
extern char *path;
extern char *imgPath;
extern char *imgExt;
extern char *GTK_FACE_AUTHENTICATE;
extern char *XAUTH_EXTRACT_FILE;
extern char *XAUTHDISPLAY_EXTRACT_FILE;
extern char *XML_GTK_BUILDER_FACE_MANAGER;
extern char *XML_GTK_BUILDER_FACE_AUTHENTICATE;
extern char *GTK_FACE_MANAGER_KEY;
