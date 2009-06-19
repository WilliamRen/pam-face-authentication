/*
    Utility Functions
    Copyright (C) 2009 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus
    http://code.google.com/p/pam-face-authentication/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <math.h>
#include <float.h>
#include <limits.h>
#include "cv.h"
#include <QtGui>
#include "highgui.h"
#include "pam_face_defines.h"

void rotatePoint(CvPoint* srcP,CvPoint* dstP,double angle,float centreX, float centreY)
{
    printf("%e ANG1\n",angle);
    CvPoint2D32f p1;
    CvPoint2D32f p2;
    p1.x=(*srcP).x;
    p1.y=(*srcP).y;
        printf("%e %e p1 p2 \n", p1.x, p1.y);

    CvMat src = cvMat( 1, 1, CV_32FC2, &p1 );
    CvMat dst = cvMat( 1, 1, CV_32FC2, &p2 );
    CvMat *translate = cvCreateMat(2, 3, CV_32FC1);
    CvPoint2D32f centre;
    centre.x = centreX;
    centre.y = centreY;
        printf("%e %e CENTER @p1 p2 \n", centre.x, centre.y);

    cv2DRotationMatrix(centre, angle, 1.0, translate);
    cvTransform( &src, &dst, translate, NULL );
    printf("%e %e  @p1 p2 \n", p2.x, p2.y);
    double v1=floor((p2.x));
    double v2=floor((p2.y));
    (*dstP).x=int(v1);
    (*dstP).y=int(v2);
    printf("%d %d DST p1 p2 \n", (*dstP).x, (*dstP).y);
    cvReleaseMat(&translate);

}

void rotate(double angle, float centreX, float centreY,IplImage * img,IplImage * dstimg)
{
    CvPoint2D32f centre;
    CvMat *translate = cvCreateMat(2, 3, CV_32FC1);
    centre.x = centreX;
    centre.y = centreY;
    cv2DRotationMatrix(centre, angle, 1.0, translate);
    cvWarpAffine(img,dstimg,translate,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS,cvScalarAll(0));
    cvReleaseMat(&translate);
}


double CenterofMass(IplImage* src,int flagXY)
{

//    cvSaveImage("/home/darksid3hack0r/aa.jpg",src);
    CvPixelPosition8u pos_src;
    //    printf("%d %d \n",src->width,src->height);
    CV_INIT_PIXEL_POS(pos_src,(unsigned char *) src->imageData,src->widthStep,cvGetSize(src),0,0,src->origin);
    uchar * ptr_src;
    int x=0;
    int y=0;
    uchar Intensity;
    double sumPixels[400];
    double totalX2=0;
    double totalX1=0;
    double totalX0=0;
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




QImage *QImageIplImageCvt(IplImage *input)
{
    if (!input)
        return 0;

    QImage *image = new QImage(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB32);
    QRgb value;
    uchar* pBits         = image->bits();
    int nBytesPerLine    = image->bytesPerLine();

    int n,m;
    for (n=0;n<IMAGE_HEIGHT;n++)
    {
        for (m= 0;m<IMAGE_WIDTH;m++)
        {
            CvScalar s;
            s=cvGet2D(input,n,m);
            QRgb value;

            value = qRgb((uchar)s.val[2], (uchar)s.val[1], (uchar)s.val[0]);

//            printf("%d %d %d \n",(uchar)s.val[2], (uchar)s.val[1], (uchar)s.val[0]);
    uchar * scanLine = pBits+n*nBytesPerLine;
       ((uint *)scanLine)[m] = value;
//image->setPixel(m, n, value);
        }
    }



    if (input)
        cvReleaseImage(&input);

    return image;
}
