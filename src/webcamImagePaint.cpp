/** @file */

/*
    Not finished webcam paint class
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
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include "webcamImagePaint.h"

webcamImagePaint::webcamImagePaint()
{
}

void  webcamImagePaint::paintEllipse(IplImage *image,CvPoint leftEye,CvPoint rightEye)
{
    IplImage* imgDest = cvCreateImage( cvSize(image->width,image->height),8,3);
    cvZero(imgDest);
    double yvalue=rightEye.y-leftEye.y;
    double xvalue=rightEye.x-leftEye.x;
    double ang= -atan(yvalue/xvalue)*(180/CV_PI);
    double width= sqrt(pow(xvalue,2)+pow(yvalue,2));
    double ratio=sqrt(pow(xvalue,2)+pow(yvalue,2))/80;
    int length=sqrt(pow(leftEye.x-rightEye.x,2)+pow(leftEye.y-rightEye.y,2));

    CvPoint p2;

    p2.x=leftEye.x;
    p2.y=leftEye.y;

    p2.x+=width/2;
    p2.y+=35*ratio;
    CvPoint2D32f centre;
    CvMat *rotateMatrix = cvCreateMat(2, 3, CV_32FC1);
    centre.x = leftEye.x;
    centre.y = leftEye.y;
    cv2DRotationMatrix(centre, ang, 1.0, rotateMatrix);

    p2.x= floor(p2.x*CV_MAT_ELEM(*rotateMatrix, float, 0, 0) +  p2.y*CV_MAT_ELEM(*rotateMatrix, float, 0, 1) +CV_MAT_ELEM(*rotateMatrix, float, 0, 2));
    p2.y= floor(p2.x*CV_MAT_ELEM(*rotateMatrix, float, 1, 0) +  p2.y*CV_MAT_ELEM(*rotateMatrix, float, 1, 1) +CV_MAT_ELEM(*rotateMatrix, float, 1, 2));

    cvEllipse(imgDest, p2, cvSize((width/2)+ int(55*ratio),120*ratio),ang, 0, 360, cvScalar(255,255,255), -1, 8,0);
    int i,j;
    for (i=0;i<image->height;i++)
    {
        for (j=0;j<image->width;j++)
        {
            CvScalar s;
            s=cvGet2D(imgDest,i,j);
            if (s.val[0]!=255)
            {
                CvScalar s;
                s=cvGet2D(image,i,j);
                s.val[0]=s.val[0]*.45;
                s.val[1]=s.val[1]*.45;
                s.val[2]=s.val[2]*.45;
                cvSet2D(image,i,j,s);
            }



        }
    }
cvReleaseImage(&imgDest);
cvReleaseMat(&rotateMatrix);

}
void  webcamImagePaint::paintCyclops(IplImage *image,CvPoint leftEye,CvPoint rightEye)
{
    IplImage* imgDest = cvCreateImage( cvSize(image->width,image->height),8,3);
    cvCopy(image,imgDest);

    double yvalue=rightEye.y-leftEye.y;
    double xvalue=rightEye.x-leftEye.x;
    double ang= -atan(yvalue/xvalue)*(180/CV_PI);
    double width= sqrt(pow(xvalue,2)+pow(yvalue,2));
    double ratio=sqrt(pow(xvalue,2)+pow(yvalue,2))/80;
    int length=sqrt(pow(leftEye.x-rightEye.x,2)+pow(leftEye.y-rightEye.y,2));

    CvPoint p1LeftTop,p1LeftBottom,p1RightTop,p1RightBottom;

    p1LeftTop.x=leftEye.x;
    p1LeftTop.y=leftEye.y;

    p1LeftBottom.x=leftEye.x;
    p1LeftBottom.y=leftEye.y;


    p1RightTop.x=leftEye.x;
    p1RightTop.y=leftEye.y;

    p1RightBottom.x=leftEye.x;
    p1RightBottom.y=leftEye.y;



    p1LeftTop.x-=int(35*ratio);
    p1LeftTop.y-=int(35*ratio);

    p1LeftBottom.x-=int(35*ratio);
    p1LeftBottom.y+=int(35*ratio);

    p1RightTop.x+=int(width) + int(35*ratio);
    p1RightTop.y-=int(35*ratio);
    p1RightBottom.x+=int(width) + int(35*ratio);
    p1RightBottom.y+=int(35*ratio);


    CvPoint2D32f centre;
    CvMat *rotateMatrix = cvCreateMat(2, 3, CV_32FC1);
    centre.x = leftEye.x;
    centre.y = leftEye.y;
    cv2DRotationMatrix(centre, ang, 1.0, rotateMatrix);




    p1LeftTop.x= floor(p1LeftTop.x*CV_MAT_ELEM(*rotateMatrix, float, 0, 0) +  p1LeftTop.y*CV_MAT_ELEM(*rotateMatrix, float, 0, 1) +CV_MAT_ELEM(*rotateMatrix, float, 0, 2));
    p1LeftTop.y= floor(p1LeftTop.x*CV_MAT_ELEM(*rotateMatrix, float, 1, 0) +  p1LeftTop.y*CV_MAT_ELEM(*rotateMatrix, float, 1, 1) +CV_MAT_ELEM(*rotateMatrix, float, 1, 2));

    p1LeftBottom.x= floor(p1LeftBottom.x*CV_MAT_ELEM(*rotateMatrix, float, 0, 0) +  p1LeftBottom.y*CV_MAT_ELEM(*rotateMatrix, float, 0, 1) +CV_MAT_ELEM(*rotateMatrix, float, 0, 2));
    p1LeftBottom.y= floor(p1LeftBottom.x*CV_MAT_ELEM(*rotateMatrix, float, 1, 0) +  p1LeftBottom.y*CV_MAT_ELEM(*rotateMatrix, float, 1, 1) +CV_MAT_ELEM(*rotateMatrix, float, 1, 2));

    p1RightTop.x= floor(p1RightTop.x*CV_MAT_ELEM(*rotateMatrix, float, 0, 0) +  p1RightTop.y*CV_MAT_ELEM(*rotateMatrix, float, 0, 1) +CV_MAT_ELEM(*rotateMatrix, float, 0, 2));
    p1RightTop.y= floor(p1RightTop.x*CV_MAT_ELEM(*rotateMatrix, float, 1, 0) +  p1RightTop.y*CV_MAT_ELEM(*rotateMatrix, float, 1, 1) +CV_MAT_ELEM(*rotateMatrix, float, 1, 2));

    p1RightBottom.x= floor(p1RightBottom.x*CV_MAT_ELEM(*rotateMatrix, float, 0, 0) +  p1RightBottom.y*CV_MAT_ELEM(*rotateMatrix, float, 0, 1) +CV_MAT_ELEM(*rotateMatrix, float, 0, 2));
    p1RightBottom.y= floor(p1RightBottom.x*CV_MAT_ELEM(*rotateMatrix, float, 1, 0) +  p1RightBottom.y*CV_MAT_ELEM(*rotateMatrix, float, 1, 1) +CV_MAT_ELEM(*rotateMatrix, float, 1, 2));

    //printf("%d %d %d %d %d %d  %d %d\n",p1LeftTop.x,p1LeftTop.y,p1LeftBottom.x,p1LeftBottom.y,p1RightTop.x,p1RightTop.y,p1RightBottom.x,p1RightBottom.y);
    CvLineIterator iterator1;
    CvLineIterator iterator2;
    if (!(p1LeftTop.y>=0 && p1LeftTop.y<image->height && p1LeftTop.x>=0 && p1LeftTop.x<image->width &&p1LeftBottom.y>=0 && p1LeftBottom.y<image->height && p1LeftBottom.x>=0 && p1LeftBottom.x<image->width &&p1RightTop.y>=0 && p1RightTop.y<image->height && p1RightTop.x>=0 && p1RightTop.x<image->width  &&p1RightBottom.y>=0 && p1RightBottom.y<image->height && p1RightBottom.x>=0 && p1RightBottom.x<image->width))
        return;

    int count1 = cvInitLineIterator( image, p1LeftTop, p1LeftBottom, &iterator1, 8, 0 );
    int count2 = cvInitLineIterator( image, p1RightTop, p1RightBottom, &iterator2, 8, 0 );
    int countVert=count2;
    if (count1!=count2)
        countVert=count1>count2 ?count2:count1;

    static int lineVertNum=0;
    static int	 switchVal=0;

    if (switchVal==0)
        lineVertNum++;
    else
        lineVertNum--;


    if (lineVertNum==10)
        switchVal=1-switchVal;

    if (lineVertNum==0)
        switchVal=1-switchVal;

    int lineHorizontal=int(((double)lineVertNum/10.0)*(double)countVert);
    int sizeOfScanLine=countVert/7;


    for ( int i = 0; i < countVert; i++ )
    {

        CV_NEXT_LINE_POINT(iterator1);
        CV_NEXT_LINE_POINT(iterator2);
        int offset, x1, y1,x2,y2;
        offset = iterator1.ptr - (uchar*)(image->imageData);
        y1 = offset/image->widthStep;
        x1 = (offset - y1*image->widthStep)/(3*sizeof(uchar));
        offset = iterator2.ptr - (uchar*)(image->imageData);
        y2 = offset/image->widthStep;
        x2 = (offset - y2*image->widthStep)/(3*sizeof(uchar));
        int yoffsetl,yoffsetr;
        int val= int(pow(((5*fabs(i-countVert/2))/countVert),2));
        // COMMENT : VERY DIRTY HACK : EQUATION IS NOT EXACTLY RIGHT , i should be using val*cos(theta) , but who cares it works =P

        yoffsetr =int((double)(double(y2-y1)/double(x2-x1))*(double(val)));
        yoffsetl =int((double)(double(y2-y1)/double(x2-x1))*(double(-val)));
        //	printf("%d %d %d \n",yoffsetr,yoffsetl,val);
        y2+=yoffsetr;
        y1+=yoffsetl;

        int realX1=x1;
        int realX2=x2;
        x2-=val;
        x1+=val;




        if (y1>=0 && y1<image->height && x1>=0 && x1<image->width && y2>=0 && y2<image->height && x2>=0 && x2<image->width)
        {

            CvPoint l,r;
            l.x=x1;
            l.y=y1;
            r.x=x2;
            r.y=y2;



            CvLineIterator iterator;
            int countLine = cvInitLineIterator( image, l, r, &iterator, 8, 0 );
            int leftScanLineX,leftScanLineY;
            int rightScanLineX,rightScanLineY;

            for ( int j = 0; j < countLine; j++ )
            {
                int offset, x, y;
                CV_NEXT_LINE_POINT(iterator);
                offset = iterator.ptr - (uchar*)(image->imageData);
                y = offset/image->widthStep;
                x = (offset - y*image->widthStep)/(3*sizeof(uchar));
                double rat =  .05*(((double)((countVert/2) -abs((countVert/2)-i))/(double)(countVert/2)));
                rat=rat + fabs(double(.03*((double(double(fabs(l.x-r.x)/2) -fabs((double)(x)-double(fabs(l.x+r.x)/2)) ))/ (double((fabs(l.x-r.x)/2)))))) ;
                if (j==2)
                {
                    leftScanLineX=x;
                    leftScanLineY=y;
                }
                if (j==countLine-3)
                {
                    rightScanLineX=x;
                    rightScanLineY=y;
                }


                if (y>0 && y<(image->height-1) && x>0 && x<(image->width-1))
                {



                    CvScalar s;
                    s=cvGet2D(image,y-1,x);
                    s.val[1]=(s.val[1]*(1-rat)) + (255*rat);
                    cvSet2D(image,y-1,x,s);
                    s=cvGet2D(image,y,x-1);
                    s=cvGet2D(image,y-1,x-1);
                    s.val[1]=(s.val[1]*(1-rat)) + (255*rat);
                    cvSet2D(image,y-1,x-1,s);

                    s=cvGet2D(image,y+1,x+1);
                    s.val[1]=(s.val[1]*(1-rat)) + (255*rat);
                    cvSet2D(image,y+1,x+1,s);

                    s=cvGet2D(image,y,x-1);
                    s.val[1]=(s.val[1]*(1-rat)) + (255*rat);
                    cvSet2D(image,y,x-1,s);




                    s=cvGet2D(image,y+1,x);

                    s.val[1]=(s.val[1]*(1-rat)) + (255*rat);
                    cvSet2D(image,y+1,x,s);
                    s=cvGet2D(image,y,x+1);

                    s.val[1]=(s.val[1]*(1-rat)) + (255*rat);
                    cvSet2D(image,y,x+1,s);

                    s=cvGet2D(image,y,x);
                  //  s.val[0]*=1.2;
                  //  s.val[2]*=1.2;
                    s.val[1]=(s.val[1]*(1-rat)) + (255*rat);
                    cvSet2D(image,y,x,s);

                    if (i < (lineHorizontal-sizeOfScanLine) || i >(lineHorizontal+ sizeOfScanLine) )
                    {
                        //	s.val[0]=(s.val[0]*(1-rat)) + (255*rat);

                    }
                    else
                    {
                        double rat = 1*switchVal - (1 )*(((double)((abs((lineHorizontal-sizeOfScanLine)-i))/(double)(2*sizeOfScanLine))));
                        rat=fabs(rat);

                        s=cvGet2D(image,y,x);
                        s.val[0]=(s.val[0]*(1-rat)) + (255*rat);
                        s.val[1]=(s.val[1]*(1-rat)) + (255*rat);
                        s.val[2]=(s.val[2]*(1-rat)) + (255*rat);
                        cvSet2D(image,y,x,s);



                    }
                    int k;
                    if (switchVal==0)
                        k=1;
                    else
                        k=-1;
                    if (i==(lineHorizontal+ k*sizeOfScanLine))
                        cvLine(image,cvPoint(leftScanLineX,leftScanLineY), cvPoint(rightScanLineX,rightScanLineY), cvScalar(255,255,255),2,CV_AA,0);



//printf("%d %d %d %d \n",leftScanLineX,leftScanLineY,rightScanLineX,rightScanLineY);


                }

            }

        }

    }
cvReleaseImage(&imgDest);
cvReleaseMat(&rotateMatrix);
    //cvLine(image,p1LeftTop, p1LeftBottom, cvScalar(0,122,0),1,CV_AA,0);
//	cvLine(image,p1RightTop, p1RightBottom, cvScalar(0,122,0),1,CV_AA,0);

    /*// int blue_sum = 0, green_sum = 0, red_sum = LeftBottom0;
     int count = cvInitLineIterator( image, leftEye, rightEye, &iterator, 8, 0 );

     for( int i = 0; i < count; i++ ){
      CV_NEXT_LINE_POINT(iterator);

         offset = iterator.ptr - (uchar*)(image->imageData);
         y = offset/image->widthStep;
         x = (offset - y*image->widthStep)/(3*sizeof(uchar));
     	printf("%d %d \n",y,x);
     	CvScalar s;
     	if(y>=0 && y<image->height && x>=0 && x<image->width)
     	{
         s=cvGet2D(image,y,x);
     	s.val[1]=(s.val[1]*0.7) + (255*0.3);
         cvSet2D(image,y,x,s);
     	}
     }
      */

}
