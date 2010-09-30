/** @file */
/*

Rewritten
    Google Summer of Code Program 2009
    Rohan Anil (rohan.anil@gmail.com) ,
    Mentoring Organization: pardus
    Mentor: Onur Kucuk

    Copyright (C) 2008-2009
     Rohan Anil (rohan.anil@gmail.com) ,
     Alex Lau ( avengermojo@gmail.com)

    Google Summer of Code Program 2008
    Mentoring Organization: openSUSE
    Mentor: Alex Lau

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** @file */

/**
* Shared Memory Key Value for Image
*/
#define IPC_KEY_IMAGE 567814
/**
* Shared Memory Key Value for Communication
*/
#define IPC_KEY_STATUS 567813
/**
* Shared Memory Size for Image , 320X240
*/
#define IMAGE_SIZE 230400
/**
* Image Width of Webcam
*/
#define IMAGE_WIDTH 320
/**
* Image Height of Webcam
*/
#define IMAGE_HEIGHT 240

//DEFAULT PARAMETERS FOR MACE FILTER PEAK TO SIDE LOBE RATIO

/**
* Mace Filter Face Threshold Value
*/
#define MACE_FACE_DEFAULT 24
/**
* Mace Filter Eye Threshold Value
*/
#define MACE_EYE_DEFAULT 25
/**
* Mace Filter Inside Face Threshold Value
*/
#define MACE_INSIDE_FACE_DEFAULT 26




#define INPROGRESS 35

#define STOPPED 28
#define STARTED 21

#define CANCEL 14
#define AUTHENTICATE 7
#define DISPLAY_ERROR 1
#define EXIT_GUI 2

