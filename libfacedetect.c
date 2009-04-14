/*
    Copyright (C) 2008 Rohan Anil (rohan.anil@gmail.com) , Alex Lau ( avengermojo@gmail.com)

    Googl-face-authentication/e Summer of Code Program 2008
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
#include "cv.h"
#include "highgui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include "pam_face_defines.h"
#include "libfacedetect.h"



void intialize()
{
    cascade = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_FACE, 0, 0, 0 );
    nested_cascade = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_EYE, 0, 0, 0 );
    nested_cascade_2    = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_EYE_2, 0, 0, 0 );
    storage = cvCreateMemStorage(0);
}

void allocateMemory()
{
    cvClearMemStorage( storage );
}

double CenterofMass(IplImage* src,int flagXY)
{
    CvPixelPosition8u pos_src;
    CV_INIT_PIXEL_POS(pos_src,(unsigned char *) src->imageData,src->widthStep,cvGetSize(src),0,0,src->origin);
    uchar * ptr_src;
    int x=0;
    int y=0;
    uchar Intensity;
    double sumPixels[400];
    double totalX2;
    double totalX1;
    double totalX0;
    int to1;
    int to2;
    if (flagXY==0)
    {
        to1=src->width;
        to2=src-> height;
    }
    if (flagXY==1)
    {
        to1=src-> height;
        to2=src->width;
    }

    for ( x=0; x<to1; x++)
    {
        sumPixels[x]=0;
        for ( y=0;y<to2; y++)
        {
            if (flagXY==0) ptr_src = CV_MOVE_TO(pos_src,x,y,1);
            if (flagXY==1) ptr_src = CV_MOVE_TO(pos_src,y,x,1);

            Intensity = ptr_src[0];
            sumPixels[x]+=255-Intensity;
        }
        totalX2+= (sumPixels[x]*(x+1));
        totalX1+= (sumPixels[x]);
    }

    totalX0= totalX2/totalX1;
    return totalX0;
}




void rotate(double angle, float centreX, float centreY,IplImage * img,IplImage * dstimg)
{
    CvPoint2D32f centre;
    CvMat *translate = cvCreateMat(2, 3, CV_32FC1);
    centre.x = centreX;
    centre.y = centreY;
    cv2DRotationMatrix(centre, angle, 1.0, translate);
    //printf(" %e %e %e %e %e MVALUES \n",translate[0],translate[1],translate[2],translate[3],translate[4],translate[5]);
    cvWarpAffine(img,dstimg,translate,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS,cvScalarAll(0));
    cvReleaseMat(&translate);
}
void rotatePoint(CvPoint* srcP,CvPoint* dstP,float angle)
{
    CvPoint2D32f p1;
    CvPoint2D32f p2;
    p1.x=(*srcP).x;
    p1.y=(*srcP).y;
    CvMat src = cvMat( 1, 1, CV_32FC2, &p1 );
    CvMat dst = cvMat( 1, 1, CV_32FC2, &p2 );
    CvMat *translate = cvCreateMat(2, 3, CV_32FC1);
    CvPoint2D32f centre;
    centre.x = 0;
    centre.y = 0;
    cv2DRotationMatrix(centre, angle, 1.0, translate);
    cvTransform( &src, &dst, translate, NULL );
    (*dstP).x=(p2.x);
    (*dstP).y=(p2.y);
    cvReleaseMat(&translate);

}

int preprocess(IplImage * img,CvPoint plefteye,CvPoint prighteye,IplImage * face)
{
    IplImage* imgDest = cvCreateImage( cvSize(img->width,img->height),8,3);
    IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );

    cvZero(face);

    double yvalue=prighteye.y-plefteye.y;
    double xvalue=prighteye.x-plefteye.x;
    double ang= atan(yvalue/xvalue);
    double width=(prighteye.x-plefteye.x);
    double ratio=width/80;
    double twentyEight=28*ratio;
    double fourtyFive=45*ratio;
    double hundred10=110*ratio;
    CvPoint p1LeftTop,p1RightTop;
    p1LeftTop.x=plefteye.x-twentyEight;
    p1LeftTop.y=((plefteye.y+prighteye.y)/2)-fourtyFive;
    p1RightTop.x=prighteye.x+twentyEight;
    p1RightTop.y=((plefteye.y+prighteye.y)/2)  +hundred10;
    rotate(ang,plefteye.x ,prighteye.y,img,imgDest);
    cvCvtColor( imgDest, gray, CV_BGR2GRAY );
    cvSetImageROI(gray,cvRect(p1LeftTop.x,p1LeftTop.y,p1RightTop.x-p1LeftTop.x,p1RightTop.y-p1LeftTop.y));
    cvResize(gray,face,CV_INTER_LINEAR);
    cvResetImageROI(gray);
    return 1;
/*

    double h=(prighteye.y-(prighteye.y+plefteye.y)/2);
    double w=prighteye.x - (prighteye.x+plefteye.x)/2;
    double an= atan(h/w);
    an=an*180;
    an=an/CV_PI;
    double negFlag=0;
    if (an<0)
    {
        negFlag=1;
        an=-1*an;
        cvFlip( img, img, 0 );
        prighteye.y=img->height-prighteye.y;
        plefteye.y=img->height-plefteye.y;

    }
    IplImage* rotatedImg = cvCreateImage( cvSize(img->width*3,img->height*3),8,3);
    if (CheckImageROI(rotatedImg,img->width,img->height,img->width,img->height,1)==-1) return -1;
    cvSetImageROI(rotatedImg,cvRect(img->width,img->height,img->width,img->height));
    cvResize(img,rotatedImg,CV_INTER_LINEAR);
    cvResetImageROI(rotatedImg);
    CvPoint pLeftCordWithOut={(plefteye.x+img->width),(plefteye.y+img->height)};
    CvPoint pLeftCordTrans;
    rotatePoint(&pLeftCordWithOut,&plefteye ,an);
    CvPoint pRightCordWithOut={(prighteye.x+img->width),(prighteye.y+img->height)};
    CvPoint pRightCordTrans;
    rotatePoint(&pRightCordWithOut,&prighteye ,an);
    rotate(an,0 ,0,rotatedImg,rotatedImg);
    if (negFlag==1)
    {
        cvFlip( rotatedImg, rotatedImg, 0 );
        cvFlip( img, img, 0 );
        prighteye.y=rotatedImg->height-prighteye.y;
        plefteye.y=rotatedImg->height-plefteye.y;

    }



    double reWidth=img->width*3*ratio;
    double reHeight=img->height*3*ratio;
    if (ratio>.5  && ratio<1.75 && reWidth>0 && reHeight>0)
    {

        IplImage* resizedImg = cvCreateImage( cvSize(reWidth,reHeight),8,1);
        IplImage* gray = cvCreateImage( cvSize(img->width*3,img->height*3), 8, 1 );
        cvCvtColor( rotatedImg, gray, CV_BGR2GRAY );
        cvResize( gray, resizedImg, CV_INTER_LINEAR );
        prighteye.x*=ratio;
        prighteye.y*=ratio;
        plefteye.x*=ratio;
        plefteye.y*=ratio;
        CvPoint p1LeftTop,p1RightTop;
        p1LeftTop.x=plefteye.x-28;
        p1LeftTop.y=((plefteye.y+prighteye.y)/2)-45;
        p1RightTop.x=prighteye.x+28;
        p1RightTop.y=((plefteye.y+prighteye.y)/2)  +110;
        if (p1LeftTop.x>0 && p1LeftTop.y>0&& p1LeftTop.x<resizedImg->width && p1LeftTop.y<resizedImg->height && p1RightTop.x>0 && p1RightTop.y>0&&p1RightTop.x<resizedImg->width && p1RightTop.y<resizedImg->height && p1RightTop.y-p1LeftTop.y<resizedImg->height && p1RightTop.x-p1LeftTop.x <resizedImg->width)
        {
            if (CheckImageROI(resizedImg,p1LeftTop.x,p1LeftTop.y,p1RightTop.x-p1LeftTop.x,p1RightTop.y-p1LeftTop.y,2)!=-1)
            {
                cvSetImageROI(resizedImg,cvRect(p1LeftTop.x,p1LeftTop.y,p1RightTop.x-p1LeftTop.x,p1RightTop.y-p1LeftTop.y));
                cvResize(resizedImg,face,CV_INTER_LINEAR);
                cvResetImageROI(resizedImg);
                cvReleaseImage( &resizedImg );
                cvReleaseImage( &gray );
                cvReleaseImage( &rotatedImg );

                return 1;
            }

        }
        cvReleaseImage( &resizedImg );
        cvReleaseImage( &gray );
    }
    cvReleaseImage( &rotatedImg );
*/
}

int CheckImageROI(IplImage* img,double x, double y,double width,double height,double fun)
{
    if (img==NULL)
        return -1;
    if (((img->width)<=0) || ((img->height)<=0))
    {
        return -1;
    }

    if ((x)<=0 || (y)<=0)
    {
        return -1;
    }
    if (((width)<=0) || ((height)<=0))
    {
        return -1;
    }
    if ((width)>=(img->width) ||(height)>=(img->height))
    {
        return -1;
    }
    if ((((x)+(width))>=(img->width)) || (((y)+(height))>=(img->height)))
    {
        return -1;
    }
    return 0;
}



int faceDetect( IplImage* img,CvPoint *pLeftEye,CvPoint *pRightEye)
{
    int bothEyesDetectedApprox=0;
    (*pLeftEye).x=0;
    (*pLeftEye).y=0;
    (*pRightEye).x=0;
    (*pRightEye).y=0;

    IplImage *gray, *small_img;
    int i, j;
    int width;
    gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
                                       cvRound (img->height/scale)), 8, 1 );

    cvCvtColor( img, gray, CV_BGR2GRAY );
    cvResize( gray, small_img, CV_INTER_LINEAR );
    cvEqualizeHist( small_img, small_img );
    cvClearMemStorage( storage );

    if ( cascade )
    {
        CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
                                            1.1, 2, 0
                                            // |CV_HAAR_FIND_BIGGEST_OBJECT
                                            //  |CV_HAAR_DO_ROUGH_SEARCH
                                            //|CV_HAAR_DO_CANNY_PRUNING
                                            //|CV_HAAR_SCALE_IMAGE
                                            ,
                                            cvSize(90, 90) );
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

            CvMat small_img_roi;
            CvSeq* nested_objects;
            CvPoint center;
            CvScalar color = colors[4];
            int radius;
            width=r->width*scale;
            center.x = cvRound((r->x + r->width*0.5)*scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
            cvCircle( img, center, radius, color, 3, 8, 0 );
            cvGetSubRect( small_img, &small_img_roi, *r );
            nested_objects = cvHaarDetectObjects( &small_img_roi, nested_cascade, storage,
                                                  1.1, 2, 0
                                                  //|CV_HAAR_FIND_BIGGEST_OBJECT
                                                  //|CV_HAAR_DO_ROUGH_SEARCH
                                                  //|CV_HAAR_DO_CANNY_PRUNING
                                                  //|CV_HAAR_SCALE_IMAGE
                                                  ,
                                                  cvSize(0, 0) );
            int count=nested_objects ? nested_objects->total : 0;
            if (count==0)
            {

                nested_objects = cvHaarDetectObjects( &small_img_roi, nested_cascade_2, storage,
                                                      1.1, 2, 0
                                                      //|CV_HAAR_FIND_BIGGEST_OBJECT
                                                      //|CV_HAAR_DO_ROUGH_SEARCH
                                                      //|CV_HAAR_DO_CANNY_PRUNING
                                                      //|CV_HAAR_SCALE_IMAGE
                                                      ,
                                                      cvSize(0, 0) );
            }
            for ( j = 0; j < (nested_objects ? nested_objects->total : 0); j++ )
            {
                CvRect* nr = (CvRect*)cvGetSeqElem( nested_objects, j );
                center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
                center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
                if ((center.x-4)>0 && (center.x-4)<312 && (center.y-4)>0  && (center.y-4)<232)
                {
                    cvSetImageROI(gray,cvRect(center.x-4,center.y-4,8,8));
                    IplImage* eyeDetect = cvCreateImage(cvSize(8,8),8,1);
                    cvResize( gray,eyeDetect, CV_INTER_LINEAR ) ;
                    cvResetImageROI(gray);
                    double xCordinate=(center.x-4+CenterofMass(eyeDetect,0))*scale;
                    double yCordinate=(center.y-4+CenterofMass(eyeDetect,1))*scale;
                    cvReleaseImage( &eyeDetect );
                    if (center.x<cvRound((r->x + r->width*0.5)*scale))
                    {


                        (*pLeftEye).x=xCordinate;
                        (*pLeftEye).y=yCordinate;

                    }
                    else
                    {
                        (*pRightEye).x=xCordinate;
                        (*pRightEye).y=yCordinate;

                    }

                }
                //     radius = 4;
                //   cvCircle( img, cvPoint(xCordinate,yCordinate), radius, color, 1, 8, 0 );
            }
            if (((*pRightEye).y!=0) && ((*pRightEye).x!=0) && ((*pLeftEye).y==0) && ((*pLeftEye).x==0))
            {

                //  printf("Target Aquired \n");
                bothEyesDetectedApprox=1;
                if (widthEye!=0 && widthFace!=0)
                    (*pLeftEye).x=(*pRightEye).x-cvRound((width*widthEye)/widthFace);
                //  printf("Target Aquired  %d %d %d\n",(*pLeftEye).x,(*pRightEye).x,cvRound((width*widthEye)/widthFace));
                if ((*pLeftEye).x>0)
                {
                    (*pLeftEye).y=(*pRightEye).y;
                    if (((*pLeftEye).x-8)>0 && ((*pLeftEye).x-8)<312 && ((*pLeftEye).y-8)>0  && ((*pLeftEye).x-8)<232)
                    {

                        cvSetImageROI(gray,cvRect((*pLeftEye).x-8,(*pLeftEye).y-4,16,8));
                        IplImage* eyeDetect = cvCreateImage(cvSize(16,8),8,1);
                        cvResize( gray,eyeDetect, CV_INTER_LINEAR ) ;
                        cvResetImageROI(gray);
                        double xCordinate=((*pLeftEye).x-8+CenterofMass(eyeDetect,0))*scale;
                        double yCordinate=((*pLeftEye).y-4+CenterofMass(eyeDetect,1))*scale;
                        (*pLeftEye).x=xCordinate;
                        (*pLeftEye).y=yCordinate;
                        cvReleaseImage( &eyeDetect );
                    }
                }



            }

            if (((*pRightEye).y==0) && ((*pRightEye).x==0) && ((*pLeftEye).y!=0) && ((*pLeftEye).x!=0))
            {
                bothEyesDetectedApprox=1;
                if (widthEye!=0 && widthFace!=0)
                    (*pRightEye).x=(*pLeftEye).x+cvRound((width*widthEye)/widthFace);
                //   printf("Target Aquired IIIIII %d\n",(*pRightEye).x);

                if ((*pRightEye).x>0)
                {
                    (*pRightEye).y=(*pLeftEye).y;
                    if (((*pRightEye).x-8)>0 && ((*pRightEye).x-8)<312 && ((*pRightEye).y-8)>0  && ((*pRightEye).x-8)<232)
                    {
                        cvSetImageROI(gray,cvRect((*pRightEye).x-8,(*pRightEye).y-4,16,8));
                        IplImage* eyeDetect = cvCreateImage(cvSize(16,8),8,1);
                        cvResize( gray,eyeDetect, CV_INTER_LINEAR ) ;
                        cvResetImageROI(gray);
                        double xCordinate=((*pRightEye).x-8+CenterofMass(eyeDetect,0))*scale;
                        double yCordinate=((*pRightEye).y-4+CenterofMass(eyeDetect,1))*scale;
                        (*pRightEye).x=xCordinate;
                        (*pRightEye).y=yCordinate;
                        cvReleaseImage( &eyeDetect );
                    }
                }
            }

        }

    }

    //cvShowImage( "result", img );
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );
    if (((*pRightEye).y!=0) && ((*pRightEye).x!=0) && ((*pLeftEye).y!=0) && ((*pLeftEye).x!=0))
    {
        CvScalar color = colors[4];
        cvCircle( img, cvPoint((*pLeftEye).x,(*pLeftEye).y), 4, color, 1, 8, 0 );
        cvCircle( img, cvPoint((*pRightEye).x,(*pRightEye).y), 4, color, 1, 8, 0 );
        cvCircle( img, cvPoint((*pLeftEye).x,(*pLeftEye).y), 2, color, 1, 8, 0 );
        cvCircle( img, cvPoint((*pRightEye).x,(*pRightEye).y), 2, color, 1, 8, 0 );
//printf("%d %d %d %d\n",(*pLeftEye).x,(*pLeftEye).y,(*pRightEye).x,(*pRightEye).y);

        if (bothEyesDetectedApprox!=1)
        {
            if (cvRound(sqrt(pow((*pRightEye).y-(*pLeftEye).y,2) +pow((*pRightEye).x-(*pLeftEye).x,2)))>0)
            {
                widthEye=cvRound(sqrt(pow((*pRightEye).y-(*pLeftEye).y,2) +pow((*pRightEye).x-(*pLeftEye).x,2)));

                widthFace=width;
            }
        }

        return 1;
    }
    else
        return -1;

}
