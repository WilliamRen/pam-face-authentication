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
#include <cxcore.h>
#include <stdio.h>
#define TOTALPIXEL 16384

int peakToSideLobeRatio(CvMat*maceFilterVisualize,IplImage *img);
void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr );
CvMat *computeMace(IplImage **img,int size);
IplImage *  featureLBPSum(IplImage * img);
int checkBit(int i);
double getBIT(IplImage* img,double px,double py,double threshold);

int checkBit(int i)
{
// Well Removing patterns with bits changing consecutively  more than twice
    int j=i;
    int bit8=(i%2);
    int bit7=((i/2)%2);
    int bit6=((i/4)%2);
    int bit5=((i/8)%2);
    int bit4=((i/16)%2);
    int bit3=((i/32)%2);
    int bit2=((i/64)%2);
    int bit1=((i/128)%2);
    int bitVector[9]  =   {bit1,bit8,bit7, bit6, bit5,bit4, bit3,bit2,bit1};
    int current=bitVector[0];
    int count=0;
    for (i=0;i<9;i++)
    {
        if (current!=bitVector[i])
            count++;
        current=bitVector[i];
    }
    if (count>2)
        return -1;
    else
        return 0;
}

double getBIT(IplImage* img,double px,double py,double threshold)
{
    if (px<0 || py<0 || px>=img->width || py>=img->height)
        return 0;
    else
    {
        CvScalar s;
        s=cvGet2D(img,py,px);
        if (s.val[0]>=threshold)
            return 1;
        else
            return 0;
    }
}
IplImage *  featureLBPSum(IplImage * img)
{
    double sum=0;
    IplImage* imgLBP=cvCreateImage( cvSize(img->width,img->height), 8, img->nChannels );
    int i,j=0;
    cvZero(imgLBP);
    for (i=0;i<img->height;i++)
    {

        for (j=0;j<img->width;j++)
        {
            int p1x,p2x,p3x,p4x,p5x,p6x,p7x,p8x;
            int p1y,p2y,p3y,p4y,p5y,p6y,p7y,p8y;

            p1x=j-1;
            p1y=i-1;
            p2x=j;
            p2y=i-1;
            p3x=j+1;
            p3y=i-1;
            p4x=j+1;
            p4y=i;
            p5x=j+1;
            p5y=i+1;
            p6x=j;
            p6y=i+1;
            p7x=j-1;
            p7y=i+1;
            p8x=j-1;
            p8y=i;
            CvScalar s;
            s=cvGet2D(img,i,j);
            double bit1=128*getBIT(img,p1x,p1y,s.val[0]);
            double bit2=64*getBIT(img,p2x,p2y,s.val[0]);
            double bit3=32*getBIT(img,p3x,p3y,s.val[0]);
            double bit4=16*getBIT(img,p4x,p4y,s.val[0]);
            double bit5=8*getBIT(img,p5x,p5y,s.val[0]);
            double bit6=4*getBIT(img,p6x,p6y,s.val[0]);
            double bit7=2*getBIT(img,p7x,p7y,s.val[0]);
            double bit8=1*getBIT(img,p8x,p8y,s.val[0]);
            CvScalar s1;
            s1.val[0]=bit1+bit2+bit3+bit4+bit5+bit6+bit7+bit8;
            s1.val[1]=0;
            s1.val[2]=0;
            cvSet2D(imgLBP,i,j,s1);
        }
    }
    return imgLBP;
}


void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr )
{
    CvMat * tmp;
    CvMat q1stub, q2stub;
    CvMat q3stub, q4stub;
    CvMat d1stub, d2stub;
    CvMat d3stub, d4stub;
    CvMat * q1, * q2, * q3, * q4;
    CvMat * d1, * d2, * d3, * d4;

    CvSize size = cvGetSize(src_arr);
    CvSize dst_size = cvGetSize(dst_arr);
    int cx, cy;

    if (dst_size.width != size.width ||
            dst_size.height != size.height)
    {
        cvError( CV_StsUnmatchedSizes, "cvShiftDFT", "Source and Destination arrays must have equal sizes", __FILE__, __LINE__ );
    }

    if (src_arr==dst_arr)
    {
        tmp = cvCreateMat(size.height/2, size.width/2, cvGetElemType(src_arr));
    }

    cx = size.width/2;
    cy = size.height/2; // image center

    q1 = cvGetSubRect( src_arr, &q1stub, cvRect(0,0,cx, cy) );
    q2 = cvGetSubRect( src_arr, &q2stub, cvRect(cx,0,cx,cy) );
    q3 = cvGetSubRect( src_arr, &q3stub, cvRect(cx,cy,cx,cy) );
    q4 = cvGetSubRect( src_arr, &q4stub, cvRect(0,cy,cx,cy) );
    d1 = cvGetSubRect( src_arr, &d1stub, cvRect(0,0,cx,cy) );
    d2 = cvGetSubRect( src_arr, &d2stub, cvRect(cx,0,cx,cy) );
    d3 = cvGetSubRect( src_arr, &d3stub, cvRect(cx,cy,cx,cy) );
    d4 = cvGetSubRect( src_arr, &d4stub, cvRect(0,cy,cx,cy) );

    if (src_arr!=dst_arr)
    {
        if ( !CV_ARE_TYPES_EQ( q1, d1 ))
        {
            cvError( CV_StsUnmatchedFormats, "cvShiftDFT", "Source and Destination arrays must have the same format", __FILE__, __LINE__ );
        }
        cvCopy(q3, d1, 0);
        cvCopy(q4, d2, 0);
        cvCopy(q1, d3, 0);
        cvCopy(q2, d4, 0);
    }
    else
    {
        cvCopy(q3, tmp, 0);
        cvCopy(q1, q3, 0);
        cvCopy(tmp, q1, 0);
        cvCopy(q4, tmp, 0);
        cvCopy(q2, q4, 0);
        cvCopy(tmp, q2, 0);
    }
}
//UMACE
/*

CvMat *computeMace(IplImage **img,int size)
{
    ////printf("%d size =",size);
    IplImage ** faces=new IplImage *[size];
    IplImage ** grayfaces=new IplImage *[size];
    int index=0;
    for(index=0;index<size;index++)
        {

                faces[index]=cvCreateImage( cvSize(img[index]->width,img[index]->height), 8, 1 );
                cvCvtColor( img[index], faces[index], CV_BGR2GRAY );
                cvEqualizeHist(faces[index],faces[index]);

        }

    CvMat * D = cvCreateMat(TOTALPIXEL,1, CV_64FC2 );
    CvMat * DINV = cvCreateMat(TOTALPIXEL,1, CV_64FC2 );
    CvMat * M_UMACE = cvCreateMat(TOTALPIXEL,1, CV_64FC2 );

    CvMat * S = cvCreateMat(TOTALPIXEL,size, CV_64FC2 );
    CvMat * SPLUS = cvCreateMat(size,TOTALPIXEL, CV_64FC2 );
    CvMat * SPLUS_DINV = cvCreateMat(size,TOTALPIXEL, CV_64FC2 );
    CvMat * DINV_S = cvCreateMat(TOTALPIXEL,size, CV_64FC2 );
    CvMat * SPLUS_DINV_S = cvCreateMat(size,size, CV_64FC2 );
    CvMat * SPLUS_DINV_S_INV = cvCreateMat(size,size, CV_64FC2 );
    CvMat * SPLUS_DINV_S_INV_1 = cvCreateMat(2*size,2*size, CV_64FC1 );
    CvMat * SPLUS_DINV_S_INV_1_INV = cvCreateMat(2*size,2*size, CV_64FC1 );
    CvMat * Hmace = cvCreateMat(TOTALPIXEL,size, CV_64FC2 );
    CvMat * Cvalue = cvCreateMat(size,1, CV_64FC2 );
    CvMat * Hmace_FIN = cvCreateMat(TOTALPIXEL,1, CV_64FC2 );
int i=0,j=0;
    for (i=0;i<TOTALPIXEL;i++)
    {
        CvScalar s;
        s.val[0]=0;
        s.val[1]=0;
        cvSet2D(  D, i, 0,s);
        cvSet2D(  DINV, i, 0, s);
        cvSet2D(  M_UMACE, i, 0, s);

    }




    for (i=0;i<size;i++)
    {
        IplImage *gray = cvCreateImage( cvSize(64,64), 8, 1 );
        cvResize(faces[i], gray, CV_INTER_LINEAR ) ;
        grayfaces[i]=gray;
        CvMat tmp;

        IplImage *  realInput = cvCreateImage( cvGetSize(gray), IPL_DEPTH_64F, 1);
        IplImage *  realInputDouble = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 1);
        IplImage *  imaginaryInput = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 1);
        IplImage *  complexInput = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 2);
        cvScale(grayfaces[i], realInput, 1.0, 0.0);
        cvZero(realInputDouble);
        cvZero(imaginaryInput);
        cvGetSubRect( realInputDouble, &tmp, cvRect(0,0,64,64));
        cvCopy(realInput,&tmp);
        cvMerge(realInputDouble, imaginaryInput, NULL, NULL, complexInput);

        CvMat * dftImage = cvCreateMat( 128, 128, CV_64FC2 );
        cvGetSubRect( dftImage, &tmp, cvRect(0,0,128,128));
        cvCopy( complexInput, &tmp, NULL );
        cvDFT( dftImage, dftImage, CV_DXT_FORWARD,0);
        int l=0,m=0;
        for (l=0;l<128;l++)
        {
            for (m=0;m<128;m++)
            {
                CvScalar scalar = cvGet2D( dftImage, l, m );
                CvScalar scalar1 = cvGet2D( M_UMACE, (l*128 + m), 0 );
                scalar1.val[0]+=scalar.val[0];
                scalar1.val[1]+=scalar.val[1];
                cvSet2D(M_UMACE,(l*128 + m), 0,scalar1);
                cvSet2D(S,(l*128 + m), i,scalar);
                CvScalar scalarConj=scalar;
                scalarConj.val[1]*=-1;
                cvSet2D(SPLUS,i,(l*128 + m),scalarConj);
                double val=(pow(scalar.val[0],2)+pow(scalar.val[1],2));

                CvScalar s= cvGet2D(D,(l*128 + m),0);
                s.val[0]+=val;
                s.val[1]=0;
                cvSet2D(  D, (l*128 + m), 0,s);

            }
        }
        cvReleaseImage(&realInput);
        cvReleaseImage(&realInputDouble);
        cvReleaseImage(&imaginaryInput);
        cvReleaseImage(&complexInput);
        cvReleaseImage(&gray);
    }


    for (i=0;i<TOTALPIXEL;i++)
    {

        CvScalar s= cvGet2D(D,i,0);
        CvScalar s1= cvGet2D(M_UMACE,i,0);
        s.val[0]=(1/s.val[0]);
        s.val[1]=0;
        cvSet2D(  DINV, i, 0, s);
        s1.val[0]*=((s.val[0])*size*128*128);
        s1.val[1]*=((s.val[0])*size*128*128);
        cvSet2D(  M_UMACE, i, 0, s1);

    }
    int l=0,m=0;
    for (l=0;l<size;l++)
    {
        for (m=0;m<TOTALPIXEL;m++)
        {

            CvScalar s1= cvGet2D(DINV,m,0);
            CvScalar s2= cvGet2D(SPLUS,l,m);
            CvScalar s3= cvGet2D(S,m,l);
            s2.val[0]*=s1.val[0];
            s2.val[1]*=s1.val[0];
            s3.val[0]*=s1.val[0];
            s3.val[1]*=s1.val[0];
            cvSet2D(SPLUS_DINV, l, m,s2);
            cvSet2D(DINV_S, m, l,s3);
        }
    }

    cvMatMul(SPLUS_DINV,S,SPLUS_DINV_S);

    for (l=0;l<size;l++)
    {
        for (m=0;m<size;m++)
        {
            CvScalar s1= cvGet2D(SPLUS_DINV_S,l,m);
            CvScalar s2;
            s2.val[0]=s1.val[0];
            s2.val[1]=0;
            cvSet2D(SPLUS_DINV_S_INV_1, l, m,s2);
            cvSet2D(SPLUS_DINV_S_INV_1, l+size, m+size,s2);
            s2.val[0]=0;
            s2.val[1]=s1.val[1];
            cvSet2D(SPLUS_DINV_S_INV_1, l, m+size,s2);
            s2.val[0]=0;
            s2.val[1]=-s1.val[1];
            cvSet2D(SPLUS_DINV_S_INV_1, l+size, m,s2);
        }
    }
    cvInvert(SPLUS_DINV_S_INV_1,SPLUS_DINV_S_INV_1_INV);
    for (l=0;l<size;l++)
    {
        for (m=0;m<size;m++)
        {

            CvScalar s1= cvGet2D(SPLUS_DINV_S_INV_1_INV,l,m);
            CvScalar s2= cvGet2D(SPLUS_DINV_S_INV_1_INV,l,m+size);
            CvScalar s3;
            s3.val[0]=s1.val[0];
            s3.val[1]=s2.val[0];
            cvSet2D(SPLUS_DINV_S_INV, l, m,s3);
        }
    }

    cvMatMul(DINV_S,SPLUS_DINV_S_INV,Hmace);

    for (l=0;l<size;l++)
    {
        CvScalar s3;
        s3.val[0]=1;
        s3.val[1]=0;

        cvSet2D(Cvalue, l, 0,s3);


    }
    cvMatMul(Hmace,Cvalue,Hmace_FIN);
    CvMat *  maceFilterVisualize = cvCreateMat( 128, 128, CV_64FC2 );
    for (l=0;l<128;l++)
    {
        for (m=0;m<128;m++)
        {
            CvScalar s1= cvGet2D(M_UMACE,(l*128 +m),0);
            cvSet2D(maceFilterVisualize, l, m,s1);
        }

    }

    cvReleaseMat(&Hmace_FIN);
    cvReleaseMat(&Hmace);
    cvReleaseMat(&Cvalue);
    cvReleaseMat(&SPLUS_DINV_S_INV_1_INV);
    cvReleaseMat(&SPLUS_DINV_S_INV_1);
    cvReleaseMat(&SPLUS_DINV_S_INV);
    cvReleaseMat(&SPLUS_DINV_S);
    cvReleaseMat(&DINV_S);
    cvReleaseMat(&SPLUS_DINV);
    cvReleaseMat(&SPLUS);
    cvReleaseMat(&S);
    cvReleaseMat(&DINV);
    cvReleaseMat(&D);

return maceFilterVisualize;


}
*/

CvMat *computeMace(IplImage **img,int size)
{
    ////printf("%d size =",size);
    IplImage ** faces=new IplImage *[size];
    IplImage ** grayfaces=new IplImage *[size];
    int index=0;
    for (index=0;index<size;index++)
    {

        faces[index]=cvCreateImage( cvSize(img[index]->width,img[index]->height), 8, 1 );
        cvCvtColor( img[index], faces[index], CV_BGR2GRAY );
        //   faces[index]=featureLBPSum(faces[index]);
        cvEqualizeHist(faces[index],faces[index]);

    }

    CvMat * D = cvCreateMat(TOTALPIXEL,1, CV_64FC2 );
    CvMat * DINV = cvCreateMat(TOTALPIXEL,1, CV_64FC2 );
    CvMat * S = cvCreateMat(TOTALPIXEL,size, CV_64FC2 );
    CvMat * SPLUS = cvCreateMat(size,TOTALPIXEL, CV_64FC2 );
    CvMat * SPLUS_DINV = cvCreateMat(size,TOTALPIXEL, CV_64FC2 );
    CvMat * DINV_S = cvCreateMat(TOTALPIXEL,size, CV_64FC2 );
    CvMat * SPLUS_DINV_S = cvCreateMat(size,size, CV_64FC2 );
    CvMat * SPLUS_DINV_S_INV = cvCreateMat(size,size, CV_64FC2 );
    CvMat * SPLUS_DINV_S_INV_1 = cvCreateMat(2*size,2*size, CV_64FC1 );
    CvMat * SPLUS_DINV_S_INV_1_INV = cvCreateMat(2*size,2*size, CV_64FC1 );
    CvMat * Hmace = cvCreateMat(TOTALPIXEL,size, CV_64FC2 );
    CvMat * Cvalue = cvCreateMat(size,1, CV_64FC2 );
    CvMat * Hmace_FIN = cvCreateMat(TOTALPIXEL,1, CV_64FC2 );
    int i=0,j=0;
    for (i=0;i<TOTALPIXEL;i++)
    {
        CvScalar s;
        s.val[0]=0;
        s.val[1]=0;
        cvSet2D(  D, i, 0,s);
        cvSet2D(  DINV, i, 0, s);
    }



    for (i=0;i<size;i++)
    {
        IplImage *gray = cvCreateImage( cvSize(64,64), 8, 1 );
        cvResize(faces[i], gray, CV_INTER_LINEAR ) ;
        grayfaces[i]=gray;
        CvMat tmp;

        IplImage *  realInput = cvCreateImage( cvGetSize(gray), IPL_DEPTH_64F, 1);
        IplImage *  realInputDouble = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 1);
        IplImage *  imaginaryInput = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 1);
        IplImage *  complexInput = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 2);
        cvScale(grayfaces[i], realInput, 1.0, 0.0);
        cvZero(realInputDouble);
        cvZero(imaginaryInput);
        cvGetSubRect( realInputDouble, &tmp, cvRect(0,0,64,64));
        cvCopy(realInput,&tmp);
        cvMerge(realInputDouble, imaginaryInput, NULL, NULL, complexInput);

        CvMat * dftImage = cvCreateMat( 128, 128, CV_64FC2 );
        cvGetSubRect( dftImage, &tmp, cvRect(0,0,128,128));
        cvCopy( complexInput, &tmp, NULL );
        cvDFT( dftImage, dftImage, CV_DXT_FORWARD,0);
        int l=0,m=0;
        for (l=0;l<128;l++)
        {
            for (m=0;m<128;m++)
            {

                CvScalar scalar = cvGet2D( dftImage, l, m );
                cvSet2D(S,(l*128 + m), i,scalar);

                //   if(i==0)
                ////printf("%e %e VAL of S1 and S2 \n",scalar.val[0],scalar.val[1]);

                CvScalar scalarConj;
                scalarConj.val[0]=scalar.val[0];
                scalarConj.val[1]=-scalar.val[1];
                cvSet2D(SPLUS,i,(l*128 + m),scalarConj);
                double val=((pow(scalar.val[0],2)+pow(scalar.val[1],2)));

                CvScalar s= cvGet2D(D,(l*128 + m),0);
                s.val[0]=s.val[0]+val;
                s.val[1]=0;
                cvSet2D(  D, (l*128 + m), 0,s);

            }
        }
        cvReleaseImage(&realInput);
        cvReleaseImage(&realInputDouble);
        cvReleaseImage(&imaginaryInput);
        cvReleaseImage(&complexInput);
        cvReleaseImage(&gray);
    }


    for (i=0;i<TOTALPIXEL;i++)
    {

        CvScalar s= cvGet2D(D,i,0);
        //  //printf("%e %e   VAL of S1 and S2 \n",s.val[0],s.val[1]);

        s.val[0]=((128*128*size)/sqrt(s.val[0]));
        s.val[1]=0;
        // //printf("%e %e   VAL of S1 and S2 \n",s.val[0],s.val[1]);

        cvSet2D(  DINV, i, 0, s);
    }
    int l=0,m=0;
    for (l=0;l<size;l++)
    {
        for (m=0;m<TOTALPIXEL;m++)
        {

            CvScalar s1= cvGet2D(DINV,m,0);
            CvScalar s2= cvGet2D(SPLUS,l,m);
            CvScalar s3= cvGet2D(S,m,l);
            ////printf("%e %e   VAL of S1 and S2 \n",s2.val[0],s2.val[1]);
            s2.val[0]*=s1.val[0];
            s2.val[1]*=s1.val[0];
            ////printf("%e %e   VAL of S1 and S2 \n",s2.val[0],s2.val[1]);
            s3.val[0]*=s1.val[0];
            s3.val[1]*=s1.val[0];
            //if(l==0)
            ////printf("%e %e %e %e  VAL of S1 and S2 \n",s2.val[0],s2.val[1],s3.val[0],s3.val[1]);
            cvSet2D(SPLUS_DINV, l, m,s2);
            cvSet2D(DINV_S, m, l,s3);
        }
    }
// ?
    cvMatMul(SPLUS_DINV,S,SPLUS_DINV_S);

    for (l=0;l<size;l++)
    {
        for (m=0;m<size;m++)
        {

            CvScalar s1= cvGet2D(SPLUS_DINV_S,l,m);
            CvScalar s2;
            // if(l!=m)
            ////printf("%e %e %d %d l , m VAL of S1 and S2 \n",s1.val[0],s1.val[1],l,m);
            s2.val[0]=s1.val[0];
            s2.val[1]=0;
            cvSet2D(SPLUS_DINV_S_INV_1, l, m,s2);
            cvSet2D(SPLUS_DINV_S_INV_1, l+size, m+size,s2);
            s2.val[0]=s1.val[1];
            s2.val[1]=0;
            cvSet2D(SPLUS_DINV_S_INV_1, l, m+size,s2);
            s2.val[0]=-s1.val[1];
            s2.val[1]=0;
            cvSet2D(SPLUS_DINV_S_INV_1, l+size, m,s2);
        }
    }
    cvInvert(SPLUS_DINV_S_INV_1,SPLUS_DINV_S_INV_1_INV);
    for (l=0;l<size;l++)
    {
        for (m=0;m<size;m++)
        {

            CvScalar s1= cvGet2D(SPLUS_DINV_S_INV_1_INV,l,m);
            CvScalar s2= cvGet2D(SPLUS_DINV_S_INV_1_INV,l,m+size);
            CvScalar s3;
            s3.val[0]=s1.val[0];
            s3.val[1]=s2.val[0];
            cvSet2D(SPLUS_DINV_S_INV, l, m,s3);
            //   //printf("%e %e  S3 values\n",s3.val[0],s3.val[1]);
        }
    }

    cvMatMul(DINV_S,SPLUS_DINV_S_INV,Hmace);

    for (l=0;l<size;l++)
    {
        CvScalar s3;
        s3.val[0]=1;
        s3.val[1]=0;
        cvSet2D(Cvalue, l, 0,s3);
    }
    cvMatMul(Hmace,Cvalue,Hmace_FIN);
    CvMat *  maceFilterVisualize = cvCreateMat( 128, 128, CV_64FC2 );
    for (l=0;l<128;l++)
    {
        for (m=0;m<128;m++)
        {
            CvScalar s1= cvGet2D(Hmace_FIN,(l*128 +m),0);
            cvSet2D(maceFilterVisualize, l, m,s1);
        }

    }
    CvScalar s1= cvGet2D(Hmace_FIN,0,0);


    cvReleaseMat(&Hmace_FIN);
    cvReleaseMat(&Hmace);
    cvReleaseMat(&Cvalue);
    cvReleaseMat(&SPLUS_DINV_S_INV_1_INV);
    cvReleaseMat(&SPLUS_DINV_S_INV_1);
    cvReleaseMat(&SPLUS_DINV_S_INV);
    cvReleaseMat(&SPLUS_DINV_S);
    cvReleaseMat(&DINV_S);
    cvReleaseMat(&SPLUS_DINV);
    cvReleaseMat(&SPLUS);
    cvReleaseMat(&S);
    cvReleaseMat(&DINV);
    cvReleaseMat(&D);

    return maceFilterVisualize;


}

int peakToSideLobeRatio(CvMat*maceFilterVisualize,IplImage *img)
{
    IplImage* face=cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    cvCvtColor( img, face, CV_BGR2GRAY );
    //  face=featureLBPSum(face);
    cvEqualizeHist( face,face);
    IplImage * grayImage= cvCreateImage( cvSize(64,64),8,1);
    cvResize(face, grayImage, CV_INTER_LINEAR ) ;
    IplImage *  realInput = cvCreateImage( cvSize(64,64), IPL_DEPTH_64F, 1);
    IplImage *  realInputDouble = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 1);
    IplImage *  imaginaryInput = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 1);
    IplImage *  complexInput = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 2);

    cvScale(grayImage, realInput, 1.0, 0.0);
    cvZero(imaginaryInput);
    cvZero(realInputDouble);

    CvMat tmp;
    cvGetSubRect( realInputDouble, &tmp, cvRect(0,0,64,64));
    cvCopy(realInput,&tmp);
    cvMerge(realInputDouble, imaginaryInput, NULL, NULL, complexInput);

    CvMat * dftImage = cvCreateMat( 128, 128, CV_64FC2 );
    cvGetSubRect( dftImage, &tmp, cvRect(0,0,128,128));
    cvCopy( complexInput, &tmp, NULL );
    cvDFT( dftImage, dftImage, CV_DXT_FORWARD,0);
    cvMulSpectrums(dftImage , maceFilterVisualize, dftImage,CV_DXT_MUL_CONJ);

    cvDFT( dftImage, dftImage, CV_DXT_INV_SCALE,0 );
    IplImage * image_Re = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 1);
    IplImage * image_ReV = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 1);

    IplImage *   image_Im = cvCreateImage( cvSize(128,128), IPL_DEPTH_64F, 1);


    // Split Fourier in real and imaginary parts
    cvSplit( dftImage, image_Re, image_Im, 0, 0 );
    //cvPow( image_Re, image_Re, 2.0);
//    cvPow( image_Im, image_Im, 2.0);
    //  cvAdd( image_Re, image_Im, image_Re, NULL);
    //cvPow( image_Re, image_Re, 0.5 );
    cvShiftDFT( image_Re, image_Re );
    double m1,M1;
    CvPoint p1,p2;
    cvMinMaxLoc(image_Re, &m1, &M1, &p1, &p2, NULL);
    // //printf("min %e max  %e \n",m1,M1);
    cvScale(image_Re, image_ReV, 1.0/(M1-m1), 1.0*(-m1)/(M1-m1));
    static int count=0;
    count++;
    char a[3];
    s//printf(a,"a%d",count);
    cvNamedWindow((a),1);
    cvRectangle( image_ReV, cvPoint(44,44), cvPoint(84,84), CV_RGB(255,0,0), 3, 8, 0 );
    cvShowImage((a),image_ReV);
    cvScale(image_Re, image_Re, 1.0, 1.0*(-m1));

    cvMinMaxLoc(image_Re, &m1, &M1, &p1, &p2, NULL);
    ////printf("min %e max  %e \n",m1,M1);
    int l=0,m=0;
    double value=0;
    double num=0;
    for (l=0;l<128;l++)
    {
        for (m=0;m<128;m++)
        {

            double rad=sqrt((pow(m-64,2)+pow(l-64,2)));
            if (rad<45)
            {

                if (rad>30)
                {
                    CvScalar s1= cvGet2D(image_Re,l,m);
                    value+=s1.val[0];
                    num++;
                }


            }

        }
    }
    value=value/num;

    double std2=0;
    for (l=0;l<128;l++)
    {
        for (m=0;m<128;m++)
        {
            double rad=sqrt((pow(m-64,2)+pow(l-64,2)));
            if (rad<45)
            {

                if (rad>30)
                {
                    CvScalar s1= cvGet2D(image_Re,l,m);
                    std2+=(pow(value-s1.val[0],2));
                    //     //printf("%e \n",s1.val[0]);

                }

            }
        }
    }

    std2/=num;
    std2=(sqrt(std2));
    CvScalar sca= cvGet2D(image_Re,64,64);
    double v=(sca.val[0]-value)/std2;
    cvReleaseImage(&face);
    cvReleaseImage(&grayImage);
    cvReleaseMat(&dftImage);
    cvReleaseImage(&image_Re);
    cvReleaseImage(&image_Im);
    cvReleaseImage(&realInput);
    cvReleaseImage(&realInputDouble);
    cvReleaseImage(&imaginaryInput);
    cvReleaseImage(&complexInput);
    int val=(int)floor(v);
    return v;
}



void rotatePoint(CvPoint* srcP,CvPoint* dstP,double angle,float centreX, float centreY)
{
    //printf("%e ANG1\n",angle);
    CvPoint2D32f p1;
    CvPoint2D32f p2;
    p1.x=(*srcP).x;
    p1.y=(*srcP).y;
    //printf("%e %e p1 p2 \n", p1.x, p1.y);

    CvMat src = cvMat( 1, 1, CV_32FC2, &p1 );
    CvMat dst = cvMat( 1, 1, CV_32FC2, &p2 );
    CvMat *translate = cvCreateMat(2, 3, CV_32FC1);
    CvPoint2D32f centre;
    centre.x = centreX;
    centre.y = centreY;
    //printf("%e %e CENTER @p1 p2 \n", centre.x, centre.y);

    cv2DRotationMatrix(centre, angle, 1.0, translate);
    cvTransform( &src, &dst, translate, NULL );
    //printf("%e %e  @p1 p2 \n", p2.x, p2.y);
    double v1=floor((p2.x));
    double v2=floor((p2.y));
    (*dstP).x=int(v1);
    (*dstP).y=int(v2);
    //printf("%d %d DST p1 p2 \n", (*dstP).x, (*dstP).y);
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
    //    //printf("%d %d \n",src->width,src->height);
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

//            //printf("%d %d %d \n",(uchar)s.val[2], (uchar)s.val[1], (uchar)s.val[0]);
            uchar * scanLine = pBits+n*nBytesPerLine;
            ((uint *)scanLine)[m] = value;
//image->setPixel(m, n, value);
        }
    }




    return image;
}
