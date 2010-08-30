/*
    Face Detector class
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

// #include <cstdio>
#include <string>
#include "faceDetector.h"
#include "tracker.h"
#include "cv.h"

using std::string;

// Frontal face cascade
const string HAAR_CASCADE_FACE = PKGDATADIR "/haarcascade.xml";

//------------------------------------------------------------------------------
faceDetector::faceDetector()
{
    // Load the classifier
    cascade_ = (CvHaarClassifierCascade*)cvLoad(HAAR_CASCADE_FACE.c_str(), 0, 0, 0);
    
    // Setup the storage and clear it
    storage_ = cvCreateMemStorage(0);
    cvClearMemStorage(storage_);

    // Initialize faceInformation params
    faceInformation.LT = cvPoint(0, 0);
    faceInformation.RB = cvPoint(0, 0);
    faceInformation.Width = 0;
    faceInformation.Height = 0;
}

//------------------------------------------------------------------------------
faceDetector::~faceDetector()
{
    if(cascade_) cvReleaseHaarClassifierCascade(&cascade_);
    if(storage_) cvReleaseMemStorage(&storage_);
}

//------------------------------------------------------------------------------
void faceDetector::runFaceDetector(IplImage* input)
{
    static tracker faceTracker;
    int scale = 1, maxI = -1, max0 = 0;
    // double t = (double)cvGetTickCount();    

    // (Re-)initialize faceInformation params
    faceInformation.LT = cvPoint(0, 0);
    faceInformation.RB = cvPoint(0, 0);
    faceInformation.Width = 0;
    faceInformation.Height = 0;
    
    cvClearMemStorage(storage_);
    
    // If no image is given, return
    if(input == 0) return;

    IplImage* gray = cvCreateImage(cvGetSize(input), 8, 1);
    IplImage* small_img = cvCreateImage(cvSize(cvRound (input->width/scale),
                                     cvRound (input->height/scale)), 8, 1);

    // The classifier works on grey scale images, 
    // so the incoming BGR image input is converted to greyscale 
    // and then optionally resized.
    cvCvtColor(input, gray, CV_BGR2GRAY);
    cvResize(gray, small_img, CV_INTER_LINEAR);
    
    // Perform histogram equalization (increases contrast and dynamic range)
    cvEqualizeHist(small_img, small_img);
    
    if(cascade_)
    {
        CvSeq* faces = cvHaarDetectObjects(small_img, cascade_, storage_, 1.4, 2, 0
          // |CV_HAAR_FIND_BIGGEST_OBJECT
          // |CV_HAAR_DO_ROUGH_SEARCH
          |CV_HAAR_DO_CANNY_PRUNING
          // |CV_HAAR_SCALE_IMAGE
          ,
          cvSize(80 / scale, 80 / scale) );
        
        for(int i = 0; i < (faces ? faces->total : 0); i++)
        {
            // Create a new rectangle for the face
            CvRect* r = (CvRect*)cvGetSeqElem(faces, maxI);

            // When looping faces, select the biggest one
            if(max0 < (r->width * r->height));
            {
                max0 = (r->width * r->height);
                maxI = i;
            }
        }

        if(maxI != -1)
        {
            CvRect* r = (CvRect*)cvGetSeqElem(faces, maxI);
            
            // Set the dimensions of the face and scale them
            faceInformation.LT.x = (r->x) * scale;
            faceInformation.LT.y = (r->y) * scale;
            faceInformation.RB.x = (r->x + r->width) * scale;
            faceInformation.RB.y = (r->y + r->height) * scale;
            faceInformation.Width = (r->width) * scale;
            faceInformation.Height = (r->height) * scale;
        
            IplImage* in = clipDetectedFace(input);
            //faceTracker.setModel(in);
            /* static CvPoint fp1, fp2;
                fp1 = faceInformation.LT;
                fp2 = faceInformation.RB;
            */
            // cvRectangle(input, faceInformation.LT, faceInformation.RB, CV_RGB(255,0,0), 3, 8, 0);
        }
        // else
        //   cvRectangle(input, faceInformation.LT, faceInformation.RB, CV_RGB(0,255,0), 3, 8, 0);
    }

    cvReleaseImage(&gray);
    cvReleaseImage(&small_img);
    
    /*
    double t1 = (double)cvGetTickCount();
    printf("detection time = %gms\n", (t1-t)/((double)cvGetTickFrequency()*1000.));
    */
}

//------------------------------------------------------------------------------
IplImage* faceDetector::clipDetectedFace(IplImage* input)
{
    if(faceInformation.Width == 0 || faceInformation.Height == 0) return 0;
    
    IplImage* faceImage = cvCreateImage( 
      cvSize(faceInformation.Width, faceInformation.Height),
      IPL_DEPTH_8U, input->nChannels);
    
    cvSetImageROI(input, 
      cvRect(faceInformation.LT.x, faceInformation.LT.y, 
        faceInformation.Width, faceInformation.Height));
    
    cvResize(input, faceImage, CV_INTER_LINEAR);
    cvResetImageROI(input);
    
    return faceImage;
}

//------------------------------------------------------------------------------
bool faceDetector::checkFaceDetected()
{
    if(faceInformation.Width != 0 && faceInformation.Height != 0)
        return true;
    else
        return false;
}

