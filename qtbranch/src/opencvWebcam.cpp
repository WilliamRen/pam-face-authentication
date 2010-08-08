/*
    OpenCV Webcam CLASS
    Copyright (C) 2010 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus
    http://www.pam-face-authentication.org

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

#include "opencvWebcam.h"
#include "cv.h"
#include "highgui.h"
#include "pam_face_defines.h"

//------------------------------------------------------------------------------
opencvWebcam::opencvWebcam()
{
}

//------------------------------------------------------------------------------
opencvWebcam::~opencvWebcam()
{
}

//------------------------------------------------------------------------------
bool opencvWebcam::startCamera()
{
   int i = 0;
   CvFileStorage* fileStorage = cvOpenFileStorage(PKGDATADIR "/config.xml", 0, CV_STORAGE_READ);
   
   if(fileStorage)
   {
     i = cvReadIntByName(fileStorage, 0, "CAMERA_INDEX", 0);
     cvReleaseFileStorage(&fileStorage);
   }

   capture_ = cvCaptureFromCAM(i);
   if(!capture_) capture_ = cvCaptureFromCAM(CV_CAP_ANY);
   if(!capture_) return false;
     else return true;
}

//------------------------------------------------------------------------------
void opencvWebcam::stopCamera()
{
   if(capture_) cvReleaseCapture(&capture_);
}

//------------------------------------------------------------------------------
IplImage* opencvWebcam::queryFrame()
{
    static IplImage* originalFrame = 0;
    IplImage* frame = 0;
    IplImage* frame_copy = 0;

    originalFrame = cvQueryFrame(capture_);
    if(!originalFrame) return 0;
    
    frame = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 
      IPL_DEPTH_8U, originalFrame->nChannels);
    cvResize(originalFrame, frame, CV_INTER_LINEAR);
    if(!frame) return 0;

    frame_copy = cvCreateImage(cvSize(frame->width, frame->height),
      IPL_DEPTH_8U, frame->nChannels);
    if(frame->origin == IPL_ORIGIN_TL) cvCopy(frame, frame_copy, 0);
      else cvFlip(frame, frame_copy, 0);
    
    cvReleaseImage(&frame);

    return frame_copy;
}

