/** @file */

/*
    Face Detector Class
    Copyright (C) 2009 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus
    http://code.google.com/p/pam-face-authentication/

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

#include "faceDetector.h"
#include "cv.h"
#include "tracker.h"
#include <stdio.h>

char* HAAR_CASCADE_FACE=PKGDATADIR "/haarcascade.xml";

int faceDetector::checkFaceDetected()
{
    if (faceInformation.Width!=0 && faceInformation.Height!=0)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

faceDetector::faceDetector()
{

    cascade = (CvHaarClassifierCascade*)cvLoad(HAAR_CASCADE_FACE, 0, 0, 0 );
    storage = cvCreateMemStorage(0);
    cvClearMemStorage( storage );
    faceInformation.LT= cvPoint(0,0);
    faceInformation.RB= cvPoint(0,0);
    faceInformation.Width=0;
    faceInformation.Height=0;

}
void faceDetector::runFaceDetector(IplImage *input)
{
             double t = (double)cvGetTickCount();

    static tracker faceTracker;
    static CvPoint fp1,fp2;
    faceInformation.LT= cvPoint(0,0);
    faceInformation.RB= cvPoint(0,0);
    faceInformation.Width=0;
    faceInformation.Height=0;
    if (input==0)
        return;

    IplImage *gray, *small_img;
    int i, j;
    int scale=1;

    gray = cvCreateImage( cvSize(input->width,input->height), 8, 1 );
    small_img = cvCreateImage( cvSize( cvRound (input->width/scale),
                                       cvRound (input->height/scale)), 8, 1 );

    cvCvtColor( input, gray, CV_BGR2GRAY );
    cvResize( gray, small_img, CV_INTER_LINEAR );
    cvClearMemStorage( storage );

    if ( cascade )
    {

        CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
                                            1.4, 2, 0
                                            // |CV_HAAR_FIND_BIGGEST_OBJECT
                                            //  |CV_HAAR_DO_ROUGH_SEARCH

                                            |CV_HAAR_DO_CANNY_PRUNING
                                            //|CV_HAAR_SCALE_IMAGE
                                            ,
                                            cvSize(80/scale, 80/scale) );
        int maxI=-1;
        int max0=0;

        for ( i = 0; i < (faces ? faces->total : 0); i++ )
        {
            CvRect* r = (CvRect*)cvGetSeqElem( faces, maxI);

            if (max0<(r->width*r->height));
            {
                max0=(r->width*r->height);
                maxI=i;
            }


        }


        if (maxI!=-1)
        {
            CvRect* r = (CvRect*)cvGetSeqElem( faces, maxI);
            faceInformation.LT.x=(r->x)*scale;
            faceInformation.LT.y=(r->y)*scale;
            faceInformation.RB.x=(r->x+ r->width)*scale;
            faceInformation.RB.y=(r->y+ r->height)*scale;
            faceInformation.Width=(r->width)*scale;
            faceInformation.Height=(r->height)*scale;
            IplImage *in=clipDetectedFace(input);
            //faceTracker.setModel(in);
            fp1=faceInformation.LT;
            fp2=faceInformation.RB;
            //         cvRectangle( input, faceInformation.LT, faceInformation.RB, CV_RGB(255,0,0), 3, 8, 0 );

        }

        // else
        //  cvRectangle( input, faceInformation.LT, faceInformation.RB, CV_RGB(0,255,0), 3, 8, 0 );
    }

    cvReleaseImage(&gray);
    cvReleaseImage(&small_img);
     double t1 = (double)cvGetTickCount();

 //printf( "detection time = %gms\n",(t1-t)/((double)cvGetTickFrequency()*1000.));

}

IplImage * faceDetector::clipDetectedFace(IplImage * inputImage)

{

    if (faceInformation.Width==0 ||     faceInformation.Height ==0)
        return 0;
    IplImage * faceImage= cvCreateImage( cvSize(faceInformation.Width,faceInformation.Height),IPL_DEPTH_8U, inputImage->nChannels );
    cvSetImageROI(inputImage,cvRect(faceInformation.LT.x,faceInformation.LT.y,faceInformation.Width,faceInformation.Height));
    cvResize(inputImage,faceImage, CV_INTER_LINEAR ) ;
    cvResetImageROI(inputImage);
    return faceImage;
}


