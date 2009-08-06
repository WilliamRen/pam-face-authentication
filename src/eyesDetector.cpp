/** @file */

/*
    EYE DETECTOR CLASS
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

#include "eyesDetector.h"
#include "pam_face_defines.h"
#include <stdio.h>
#include <highgui.h>
#include <cv.h>

double CenterofMass(IplImage* src,int flagXY);

char *HAAR_CASCADE_EYE=PKGDATADIR "/haarcascade_eye_tree_eyeglasses.xml";
char *HAAR_CASCADE_EYE_2=PKGDATADIR "/haarcascade_eye.xml";

eyesDetector::eyesDetector()
{

    nested_cascade = (CvHaarClassifierCascade*)cvLoad(HAAR_CASCADE_EYE, 0, 0, 0 );
    nested_cascade_2    = (CvHaarClassifierCascade*)cvLoad(HAAR_CASCADE_EYE_2, 0, 0, 0 );
    storage = cvCreateMemStorage(0);
    cvClearMemStorage( storage );
    eyesInformation.LE =cvPoint(0,0);
    eyesInformation.RE =cvPoint(0,0);
    eyesInformation.Length =0;
    bothEyesDetected=0;


}
int eyesDetector::checkEyeDetected()
{
    if (bothEyesDetected==1)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

void eyesDetector::runEyesDetector(IplImage * input,IplImage * fullImage,CvPoint LT)

{

    bothEyesDetected=0;
    //static int countR;
    //static CvPoint leftEyeP,rightEyeP;
    eyesInformation.LE =cvPoint(0,0);
    eyesInformation.RE =cvPoint(0,0);
    eyesInformation.Length =0;
    if (input==0)
        return;

    double scale=1,i=0,j=0;
    //  //printf("%e SCALE \n\n",scale);

    IplImage *gray = cvCreateImage( cvSize(input->width,input->height/(2)), 8, 1 );
    IplImage *gray_fullimage = cvCreateImage( cvSize(fullImage->width,fullImage->height), 8, 1 );

    IplImage *gray_scale = cvCreateImage( cvSize(input->width/scale,input->height/(2*scale)), 8, 1 );

    cvSetImageROI(input,cvRect(0,(input->height)/(8),input->width,(input->height)/(2)));
    cvCvtColor( input, gray, CV_BGR2GRAY );
    cvResetImageROI(input);


    cvCvtColor( fullImage, gray_fullimage, CV_BGR2GRAY );
    cvResize(gray,gray_scale,CV_INTER_LINEAR);


    cvClearMemStorage( storage );
    CvSeq* nested_objects = cvHaarDetectObjects(gray_scale, nested_cascade, storage,1.4, 2, 0,cvSize(0,0) );
    int count=nested_objects ? nested_objects->total : 0;
    if (count==0)
    {
        nested_objects = cvHaarDetectObjects( gray_scale, nested_cascade_2, storage,1.4, 2, 0,cvSize(0,0) );
    }
    int leftT=0,rightT=0;
    count=nested_objects ? nested_objects->total : 0;
    int Flag=0;
    if (count>1)
    {

        for ( j = 0; j < (nested_objects ? nested_objects->total : 0); j++ )
        {
            CvPoint center;
            CvRect* nr = (CvRect*)cvGetSeqElem( nested_objects, j );
            center.x = cvRound((LT.x+ (nr->x + nr->width*0.5)*scale));
            center.y = cvRound((LT.y + (input->height)/8 + (nr->y + nr->height*0.5)*scale));

            if ((center.x-4)>0 && (center.x-4)<(IMAGE_WIDTH-8) && (center.y-4)>0  && (center.y-4)<(IMAGE_HEIGHT-8))
            {
                cvSetImageROI(gray_fullimage,cvRect(center.x-4,center.y-4,8,8));
                IplImage* eyeDetect = cvCreateImage(cvSize(8,8),8,1);
                cvResize( gray_fullimage,eyeDetect, CV_INTER_LINEAR ) ;
                cvResetImageROI(gray_fullimage);

                double xCordinate=(center.x-4+CenterofMass(eyeDetect,0));
                double yCordinate=(center.y-4+CenterofMass(eyeDetect,1));

                cvReleaseImage( &eyeDetect );
                if (center.x<cvRound((LT.x + input->width*0.5)))
                {
                    eyesInformation.LE.x=xCordinate;
                    eyesInformation.LE.y=yCordinate;

                    //cvCircle( fullImage, cvPoint(eyesInformation.LE.x,eyesInformation.LE.y), 4, CV_RGB(128,128,128), 1, 8, 0 );
                    leftT=1;
                }
                else
                {

                    eyesInformation.RE.x=xCordinate;
                    eyesInformation.RE.y=yCordinate;
                    //cvCircle( fullImage, cvPoint(eyesInformation.RE.x,eyesInformation.RE.y), 4, CV_RGB(128,128,128), 1, 8, 0 );

                    rightT=1;
                }

            }
        }





        if (leftT==1 && rightT==1)
        {
            eyesInformation.Length=sqrt(pow(eyesInformation.RE.y-eyesInformation.LE.y,2)+ pow(eyesInformation.RE.x-eyesInformation.LE.x,2));
            bothEyesDetected=1;
        }

    }
    cvReleaseImage(&gray_fullimage);
    cvReleaseImage(&gray);
    cvReleaseImage(&gray_scale);

}
