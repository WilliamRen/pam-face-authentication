/** @file */

/*
    Utility Functions
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

#include <math.h>
#include <float.h>
#include <limits.h>
#include "cv.h"
#include "highgui.h"

#include "pam_face_defines.h"
#include <cxcore.h>
#include <stdio.h>

#include "utils.h"


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

void  featureLBPHist(IplImage * img,CvMat *features_final)
{
    int lbpArry[256];

    IplImage* imgLBP=cvCreateImage( cvSize(img->width,img->height), 8,1 );
    int Nx = floor((img->width )/35);
    int Ny= floor((img->height)/30);

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
            // printf("%d A\n",int(s1.val[0]));
            cvSet2D(imgLBP,i,j,s1);
        }
    }
    int l,m,k=0;
    for (i=0;i<Ny;i++)
    {
        for (j=0;j<Nx;j++)
        {
            int count=0;
            CvScalar s;
            s.val[0]=0;
            for (k=0;k<256;k++)
            {
                if (checkBit(k)==0)
                {
                    cvSet2D(features_final,i*Nx*59 + j*59 +count,0,s);
                    lbpArry[k]=count;
                    count++;
                }
                else
                {
                    cvSet2D(features_final,i*Nx*59 + j*59 +58,0,s);
                    lbpArry[k]=58;
                }
            }
            int startX=35*j;
            int startY=30*i;
            for (l=0;l<30;l++)
            {

                for (m=0;m<35;m++)
                {
                    CvScalar s;
                    s=cvGet2D(imgLBP,startY+l,startX+m);
                    int val=s.val[0];
                    CvScalar s1;
                    s1=cvGet2D(features_final,i*Nx*59 + j*59 +lbpArry[val],0);
                    s1.val[0]+=1;
                    cvSet2D(features_final,i*Nx*59 + j*59+lbpArry[val],0,s1);

                }
            }


        }
    }

    cvReleaseImage( &imgLBP);
}



CvMat * createGaussianFilter(int size)
{
    CvMat * filter = cvCreateMat(size,size, CV_64FC1 );

    double sigma = (double)size/5;
    int halfsize = (int)size/2;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            int x_value = (-halfsize) + i;
            int y_value = (-halfsize) + j;
            double val=exp(-(((double) (x_value * x_value + y_value * y_value)) / sigma));
            CvScalar s;
            s.val[0]=val;
            cvSet2D( filter, i, j,s);

        }
    }

    return filter;
}

double LBPdiff(    CvMat* model,    CvMat* test)
{
    double weights[4][5]  =
    {
        { 1, 1,  3, 1,  1},
        { 1, 2,  3,2,  1},
        { 1, 2,  2, 2,  1},
        { .3, 1,  1, 1,  .3},

    };

    int i,j,k;
    double chiSquare=0;
    for (i=0;i<5;i++)
    {
        for (j=0;j<4;j++)
        {

            for (k=0;k<59;k++)
            {
                CvScalar s1,s2;
                s1=cvGet2D(model,i*4*59 + j*59 +k,0);
                s2=cvGet2D(test,i*4*59 + j*59 +k,0);
                double hist1=0,hist2=0;
                hist1=s1.val[0];
                hist2=s2.val[0];
                if ((hist1+hist2)!=0)
                chiSquare+=(weights[j][i]*(pow(hist1-hist2,2)/(hist1+hist2)));

         //   printf("%e \n",weights[i][j]);
            }

        }

    }
    return chiSquare;
}


void saveMace(mace * maceFilter,char *path)
{
    char fullpath[300];
    sprintf(fullpath,"%s/%s", path,maceFilter->maceFilterName);
   // printf("%s \n",fullpath);
    CvFileStorage *fs;
    fs = cvOpenFileStorage( fullpath, 0, CV_STORAGE_WRITE );
    cvWrite( fs, "maceFilter", maceFilter->filter, cvAttrList(0,0) );
    cvWriteInt( fs, "thresholdPSLR", maceFilter->thresholdPSLR);
    cvWriteReal( fs, "thresholdPCER", maceFilter->thresholdPCER);
    cvReleaseFileStorage( &fs );
}


////////////////  \m/ ^^ SELF QUOTIENT IMAGE \m/  ////


IplImage * SQI(CvMat * filter,IplImage *  image,int size)
{
    CvMat * newfilter = cvCreateMat(size,size, CV_64FC1 );



    int indexl=0,indexm=0;
    int indexl1=0,indexm1=0;

    int height=image->height;
    int width=image->width;
    int i=0,j=0,l=0,m=0;
    IplImage *output =cvCreateImage( cvSize(width,height), IPL_DEPTH_64F, 1);
    cvZero(output);
    IplImage *integralImage =cvCreateImage( cvSize(width+1,height+1), IPL_DEPTH_64F, 1);
    cvIntegral(image,integralImage,0,0);

    for ( i = 0; i < image->height; i++)
    {
        for ( j = 0; j <  image->width; j++)
        {


            indexl=i- int(floor((size)/2));
            if (indexl < 0)
                indexl =0;
            indexl1=i+ int(floor((size)/2));
            if (indexl1 >= height)
                indexl1 = height- 1;

            indexm=j- int(floor((size)/2));
            if (indexm < 0)
                indexm = 0;

            indexm1 =j+ int(floor((size)/2));
            if (indexm1 >= width)
                indexm1= (width)  - 1;

            indexl+=1;
            indexm+=1;
            indexl1+=1;
            indexm1+=1;


            double mean=0;
            int countGreater=0;
            int countLower=0;
            int flag=0;
            CvScalar sxy;
            sxy=cvGet2D( integralImage,  indexl, indexm);
            CvScalar sx1y;
            sx1y=cvGet2D( integralImage,  indexl1, indexm);
            CvScalar sxy1;
            sxy1=cvGet2D( integralImage,  indexl, indexm1);
            CvScalar sx1y1;
            sx1y1=cvGet2D( integralImage,  indexl1, indexm1);


            mean= double(sx1y1.val[0] -  sxy1.val[0] - sx1y.val[0] +sxy.val[0])/double((indexm1-indexm+1)*(indexl1-indexl+1));

            for ( l = 0; l < size; l++)
            {
                indexl=i+ l - int(floor((size)/2));
                if (indexl < 0)
                    indexl = abs(indexl) - 1;
                if (indexl >= height)
                    indexl = (2 * height) - indexl - 1;

                for ( m = 0; m < size; m++)
                {
                    indexm=j+ m - int(floor((size)/2));
                    if (indexm < 0)
                        indexm = abs(indexm) - 1;
                    if (indexm >= width)
                        indexm= (2 * width) - indexm - 1;
                    CvScalar s;
                    s=cvGet2D( image,  indexl, indexm);
                    if (mean>(double)s.val[0])
                        countLower++;
                    else
                        countGreater++;

                }
            }

            if (countLower>countGreater)
                flag=1;
            else
                flag=0;
            double sumv= 0.0;
            for ( l = 0; l < size; l++)
            {
                indexl=i+ l - int(floor((size)/2));
                if (indexl < 0)
                    indexl = abs(indexl) - 1;
                if (indexl >= height)
                    indexl = (2 * height) - indexl - 1;

                for ( m = 0; m < size; m++)
                {
                    indexm=j+ m - int(floor((size)/2));
                    if (indexm < 0)
                        indexm = abs(indexm) - 1;
                    if (indexm >= width)
                        indexm= (2 * width) - indexm - 1;

                    CvScalar s;
                    s=cvGet2D( image,  indexl, indexm);
                    if (((double)s.val[0]<mean && flag==0) ||((double)s.val[0]>mean && flag==1))
                    {
                        CvScalar s1;
                        s1.val[0]=0;
                        cvSet2D( newfilter,  l, m,s1);
                    }
                    else
                    {
                        CvScalar s;
                        s=cvGet2D(filter,  l, m);
                        cvSet2D( newfilter,  l, m,s);
                    }

                    s=cvGet2D(newfilter,  l, m);

                    sumv+=s.val[0];

                }
            }




            //Convolve
            for ( l = 0; l < size; l++)
            {
                indexl=i+ l - int(floor((size)/2));
                if (indexl < 0)
                    indexl = abs(indexl) - 1;
                if (indexl >= height)
                    indexl = (2 * height) - indexl - 1;

                for ( m = 0; m < size; m++)
                {
                    indexm=j+ m - int(floor((size)/2));
                    if (indexm < 0)
                        indexm = abs(indexm) - 1;
                    if (indexm >= width)
                        indexm= (2 * width) - indexm - 1;

                    CvScalar s;
                    s= cvGet2D( output,  i, j);

                    CvScalar s1;
                    s1=cvGet2D( image,  indexl, indexm);
                    CvScalar s2;
                    s2=cvGet2D( newfilter,  l, m);
                    s.val[0]+=((s1.val[0]*s2.val[0])/sumv);
                    cvSet2D( output,  i, j,s);
                }
            }
        }
    }
    return output;
}
////////////////  \m/ ^^ SELF QUOTIENT IMAGE \m/  TODO: DO IT FASTER (CACHE)////

void createSQI(IplImage * im,IplImage *final)
{
    CvMat *a =createGaussianFilter(3);
    CvMat *b =createGaussianFilter(9);
    CvMat *c =createGaussianFilter(15);
    IplImage * ima =SQI(a,im,3);
    IplImage * imb= SQI(b,im,9);
    IplImage * imc= SQI(c,im,15);
    IplImage *final1 =cvCreateImage( cvSize(im->width,im->height), IPL_DEPTH_64F, 1);
    int i,j;
    for ( i = 0; i < im->height; i++)
    {
        for ( j = 0; j <  im->width; j++)
        {


            CvScalar s1;
            s1=cvGet2D( im,  i, j);

            CvScalar s2;
            s2=cvGet2D( ima,  i, j);

            CvScalar s3;
            s3=cvGet2D( imb,  i, j);

            CvScalar s4;
            s4=cvGet2D( imc,  i, j);

            CvScalar s5;
            s5.val[0] = log(double(s1.val[0]+1)/double(s2.val[0]+1)) +log(double(s1.val[0]+1)/double(s3.val[0]+1)) +log(double(s1.val[0]+1)/double(s4.val[0]+1));
            cvSet2D( final1,  i, j,s5);

        }
    }
    double m1,M1;
    CvPoint p1,p2;
    cvMinMaxLoc(final1, &m1, &M1, &p1, &p2, NULL);
    cvScale(final1,final1, 255.0/(M1-m1), 255.0*((-m1)/(M1-m1)));
    for ( i = 0; i < im->height; i++)
    {
        for ( j = 0; j <  im->width; j++)
        {

            CvScalar s5;
            s5=  cvGet2D( final1, i,j);
            cvSet2D( final,  i, j,s5);
        }
    }
    cvReleaseImage(&final1);

    cvReleaseImage(&ima);
    cvReleaseImage(&imb);
    cvReleaseImage(&imc);
    cvReleaseMat(&a);
    cvReleaseMat(&b);
    cvReleaseMat(&c);
//return final;

}


////////////////  \m/ ^^ SELF QUOTIENT IMAGE \m/  ////



int file_exists(const char* filename)
{
    FILE* file;
    if (file=fopen(filename,"r"))
    {
        fclose(file);
        return 1;
    }
    return 0;
}



void setConfig(config *configuration,char * configDirectory)
{
    char maceConfig[300];
    sprintf(maceConfig,"%s/mace.xml", configDirectory);
    CvFileStorage* fs ;
    fs = cvOpenFileStorage( maceConfig, 0, CV_STORAGE_WRITE );
   // printf("%e \n",configuration->percentage);
    cvWriteReal( fs, "percentage", configuration->percentage );
    cvReleaseFileStorage( &fs );
}

config * getConfig(char *configDirectory)
{
    config * newConfig=new config;
    char maceConfig[300];
    sprintf(maceConfig,"%s/mace.xml", configDirectory);
    CvFileStorage * fileStorage;
    fileStorage = cvOpenFileStorage(maceConfig, 0, CV_STORAGE_READ );
    newConfig->percentage=cvReadRealByName( fileStorage, 0, "percentage", 1);
    cvReleaseFileStorage( &fileStorage );
    return newConfig;
}

void logOfImage(IplImage * img,IplImage *logImage)
{

    int i=0;
    int j=0;
    for (i=0;i<img->height;i++)
    {

        for (j=0;j<img->width;j++)
        {

            CvScalar s;
            s=cvGet2D(img,i,j);
            CvScalar s1;
            s1.val[0]=log(s.val[0] + 1);
            s1.val[1]=log(s.val[1] + 1);
            s1.val[2]=log(s.val[2] + 1);
            cvSet2D(logImage,i,j,s1);
        }
    }

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
        IplImage *gray = cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE), 8, 1 );
        cvResize(faces[i], gray, CV_INTER_LINEAR ) ;
        grayfaces[i]=gray;
        CvMat tmp;

        IplImage *  realInput = cvCreateImage( cvGetSize(gray), IPL_DEPTH_64F, 1);
        IplImage *  realInputDouble = cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE), IPL_DEPTH_64F, 1);
        IplImage *  imaginaryInput = cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE), IPL_DEPTH_64F, 1);
        IplImage *  complexInput = cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE), IPL_DEPTH_64F, 2);
        cvScale(grayfaces[i], realInput, 1.0, 0.0);
        cvZero(realInputDouble);
        cvZero(imaginaryInput);
        cvGetSubRect( realInputDouble, &tmp, cvRect(0,0,SIZE_OF_IMAGE,SIZE_OF_IMAGE));
        cvCopy(realInput,&tmp);
        cvMerge(realInputDouble, imaginaryInput, NULL, NULL, complexInput);

        CvMat * dftImage = cvCreateMat( SIZE_OF_IMAGE, SIZE_OF_IMAGE, CV_64FC2 );
        cvGetSubRect( dftImage, &tmp, cvRect(0,0,SIZE_OF_IMAGE,SIZE_OF_IMAGE));
        cvCopy( complexInput, &tmp, NULL );
        cvDFT( dftImage, dftImage, CV_DXT_FORWARD,0);
        int l=0,m=0;
        for (l=0;l<SIZE_OF_IMAGE;l++)
        {
            for (m=0;m<SIZE_OF_IMAGE;m++)
            {
                CvScalar scalar = cvGet2D( dftImage, l, m );
                CvScalar scalar1 = cvGet2D( M_UMACE, (l*SIZE_OF_IMAGE + m), 0 );
                scalar1.val[0]+=scalar.val[0];
                scalar1.val[1]+=scalar.val[1];
                cvSet2D(M_UMACE,(l*SIZE_OF_IMAGE + m), 0,scalar1);
                cvSet2D(S,(l*SIZE_OF_IMAGE + m), i,scalar);
                CvScalar scalarConj=scalar;
                scalarConj.val[1]*=-1;
                cvSet2D(SPLUS,i,(l*SIZE_OF_IMAGE + m),scalarConj);
                double val=(pow(scalar.val[0],2)+pow(scalar.val[1],2));

                CvScalar s= cvGet2D(D,(l*SIZE_OF_IMAGE + m),0);
                s.val[0]+=val;
                s.val[1]=0;
                cvSet2D(  D, (l*SIZE_OF_IMAGE + m), 0,s);

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
        s1.val[0]*=((s.val[0])*size*SIZE_OF_IMAGE*SIZE_OF_IMAGE);
        s1.val[1]*=((s.val[0])*size*SIZE_OF_IMAGE*SIZE_OF_IMAGE);
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

    for (l=0;l<size;l++)    int  SIZE_OF_IMAGE_2X =SIZE_OF_IMAGE*2;
    int  TOTALPIXEL =SIZE_OF_IMAGE_2X*SIZE_OF_IMAGE_2X;
    ////printf("%d size =",size);
    IplImage ** faces=new IplImage *[size];
    IplImage ** grayfaces=new IplImage *[size];
    int index=0;
    for (index=0;index<size;index++)
    {

        faces[index]=cvCreateImage( cvSize(img[index]->width,img[index]->height), 8, 1 );
      //  cvResize(img[index], faces[index], CV_INTER_LINEAR );
         cvCvtColor( img[index], faces[index], CV_BGR2GRAY );
//          faces[index]=featureLBPSum(faces[index]);

        //  logOfImage(faces[index],faces[index]);
        // cvEqualizeHist(faces[index],faces[index]);

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
        IplImage *gray = cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE), 8, 1 );
        cvResize(faces[i], gray, CV_INTER_LINEAR ) ;
	cvEqualizeHist( gray,gray);
        grayfaces[i]=gray;
        CvMat tmp;

        IplImage *  realInput = cvCreateImage( cvGetSize(gray), IPL_DEPTH_64F, 1);
        IplImage *  realInputDouble = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
        IplImage *  imaginaryInput = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
        IplImage *  complexInput = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 2);
        cvScale(grayfaces[i], realInput, 1.0, 0.0);
        cvZero(realInputDouble);
        cvZero(imaginaryInput);
        cvGetSubRect( realInputDouble, &tmp, cvRect(0,0,SIZE_OF_IMAGE,SIZE_OF_IMAGE));
        cvCopy(realInput,&tmp);
        cvMerge(realInputDouble, imaginaryInput, NULL, NULL, complexInput);

        CvMat * dftImage = cvCreateMat( SIZE_OF_IMAGE_2X, SIZE_OF_IMAGE_2X, CV_64FC2 );
        cvGetSubRect( dftImage, &tmp, cvRect(0,0,SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X));
        cvCopy( complexInput, &tmp, NULL );
        cvDFT( dftImage, dftImage, CV_DXT_FORWARD,0);
        int l=0,m=0;
        for (l=0;l<SIZE_OF_IMAGE_2X;l++)
        {
            for (m=0;m<SIZE_OF_IMAGE_2X;m++)
            {

                CvScalar scalar = cvGet2D( dftImage, l, m );
                cvSet2D(S,(l*SIZE_OF_IMAGE_2X + m), i,scalar);

                //   if(i==0)
                ////printf("%e %e VAL of S1 and S2 \n",scalar.val[0],scalar.val[1]);

                CvScalar scalarConj;
                scalarConj.val[0]=scalar.val[0];
                scalarConj.val[1]=-scalar.val[1];
                cvSet2D(SPLUS,i,(l*SIZE_OF_IMAGE_2X + m),scalarConj);
                double val=((pow(scalar.val[0],2)+pow(scalar.val[1],2)));

                CvScalar s= cvGet2D(D,(l*SIZE_OF_IMAGE_2X + m),0);
                s.val[0]=s.val[0]+val;
                s.val[1]=0;
                cvSet2D(  D, (l*SIZE_OF_IMAGE_2X + m), 0,s);

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

        s.val[0]=((SIZE_OF_IMAGE_2X*SIZE_OF_IMAGE_2X*size)/sqrt(s.val[0]));
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
    CvMat *  maceFilterVisualize = cvCreateMat( SIZE_OF_IMAGE_2X, SIZE_OF_IMAGE_2X, CV_64FC2 );
    for (l=0;l<SIZE_OF_IMAGE_2X;l++)
    {
        for (m=0;m<SIZE_OF_IMAGE_2X;m++)
        {
            CvScalar s1= cvGet2D(Hmace_FIN,(l*SIZE_OF_IMAGE_2X +m),0);
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


    {
        CvScalar s3;
        s3.val[0]=1;
        s3.val[1]=0;

        cvSet2D(Cvalue, l, 0,s3);


    }
    cvMatMul(Hmace,Cvalue,Hmace_FIN);
    CvMat *  maceFilterVisualize = cvCreateMat( SIZE_OF_IMAGE, SIZE_OF_IMAGE, CV_64FC2 );
    for (l=0;l<SIZE_OF_IMAGE;l++)
    {
        for (m=0;m<SIZE_OF_IMAGE;m++)
        {
            CvScalar s1= cvGet2D(M_UMACE,(l*SIZE_OF_IMAGE +m),0);
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

CvMat *computeMace(IplImage **img,int size,int SIZE_OF_IMAGE)

{
    int  SIZE_OF_IMAGE_2X =SIZE_OF_IMAGE*2;
    int  TOTALPIXEL =SIZE_OF_IMAGE_2X*SIZE_OF_IMAGE_2X;
    ////printf("%d size =",size);
    IplImage ** faces=new IplImage *[size];
    IplImage ** grayfaces=new IplImage *[size];
    int index=0;
    for (index=0;index<size;index++)
    {

        faces[index]=cvCreateImage( cvSize(img[index]->width,img[index]->height), 8, 1 );
        //  cvResize(img[index], faces[index], CV_INTER_LINEAR );
        cvCvtColor( img[index], faces[index], CV_BGR2GRAY );
//          faces[index]=featureLBPSum(faces[index]);

        //  logOfImage(faces[index],faces[index]);
        // cvEqualizeHist(faces[index],faces[index]);

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
        IplImage *gray = cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE), 8, 1 );
        cvResize(faces[i], gray, CV_INTER_LINEAR ) ;
        cvEqualizeHist( gray,gray);
        grayfaces[i]=gray;
        CvMat tmp;

        IplImage *  realInput = cvCreateImage( cvGetSize(gray), IPL_DEPTH_64F, 1);
        IplImage *  realInputDouble = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
        IplImage *  imaginaryInput = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
        IplImage *  complexInput = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 2);

        cvScale(grayfaces[i], realInput, 1.0, 0.0);
        cvZero(realInputDouble);
        cvZero(imaginaryInput);
        cvGetSubRect( realInputDouble, &tmp, cvRect(0,0,SIZE_OF_IMAGE,SIZE_OF_IMAGE));
        cvCopy(realInput,&tmp);
        cvMerge(realInputDouble, imaginaryInput, NULL, NULL, complexInput);

        CvMat * dftImage = cvCreateMat( SIZE_OF_IMAGE_2X, SIZE_OF_IMAGE_2X, CV_64FC2 );
        cvGetSubRect( dftImage, &tmp, cvRect(0,0,SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X));
        cvCopy( complexInput, &tmp, NULL );
        cvDFT( dftImage, dftImage, CV_DXT_FORWARD,0);
        int l=0,m=0;
        for (l=0;l<SIZE_OF_IMAGE_2X;l++)
        {
            for (m=0;m<SIZE_OF_IMAGE_2X;m++)
            {

                CvScalar scalar = cvGet2D( dftImage, l, m );
                cvSet2D(S,(l*SIZE_OF_IMAGE_2X + m), i,scalar);

                //   if(i==0)
                ////printf("%e %e VAL of S1 and S2 \n",scalar.val[0],scalar.val[1]);

                CvScalar scalarConj;
                scalarConj.val[0]=scalar.val[0];
                scalarConj.val[1]=-scalar.val[1];
                cvSet2D(SPLUS,i,(l*SIZE_OF_IMAGE_2X + m),scalarConj);
                double val=((pow(scalar.val[0],2)+pow(scalar.val[1],2)));

                CvScalar s= cvGet2D(D,(l*SIZE_OF_IMAGE_2X + m),0);
                s.val[0]=s.val[0]+val;
                s.val[1]=0;
                cvSet2D(  D, (l*SIZE_OF_IMAGE_2X + m), 0,s);

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

        s.val[0]=((SIZE_OF_IMAGE_2X*SIZE_OF_IMAGE_2X*size)/sqrt(s.val[0]));
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
    CvMat *  maceFilterVisualize = cvCreateMat( SIZE_OF_IMAGE_2X, SIZE_OF_IMAGE_2X, CV_64FC2 );
    for (l=0;l<SIZE_OF_IMAGE_2X;l++)
    {
        for (m=0;m<SIZE_OF_IMAGE_2X;m++)
        {
            CvScalar s1= cvGet2D(Hmace_FIN,(l*SIZE_OF_IMAGE_2X +m),0);
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

/*
CvMat *computeMace(IplImage **img,int size,int  SIZE_OF_IMAGE)
{
    int  SIZE_OF_IMAGE_2X =SIZE_OF_IMAGE*2;
    int  TOTALPIXEL =SIZE_OF_IMAGE_2X*SIZE_OF_IMAGE_2X;
    ////printf("%d size =",size);
    IplImage ** faces=new IplImage *[size];
    IplImage ** grayfaces=new IplImage *[size];
    int index=0;
    for (index=0;index<size;index++)
    {

        faces[index]=cvCreateImage( cvSize(img[index]->width,img[index]->height), 8, 1 );
        //  cvResize(img[index], faces[index], CV_INTER_LINEAR );
        cvCvtColor( img[index], faces[index], CV_BGR2GRAY );
//          faces[index]=featureLBPSum(faces[index]);

        // cvEqualizeHist(faces[index],faces[index]);

    }
    CvMat * maxEnergy = cvCreateMat(TOTALPIXEL,1, CV_64FC2 );
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
        cvSet2D(  maxEnergy, i, 0,s);
        cvSet2D(  D, i, 0,s);
        cvSet2D(  DINV, i, 0, s);
    }


    for (i=0;i<size;i++)
    {
        IplImage *gray = cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE), 8, 1 );
        cvResize(faces[i], gray, CV_INTER_LINEAR ) ;
        cvEqualizeHist(gray,gray);

        grayfaces[i]=gray;
        CvMat tmp;

        IplImage *  realInput = cvCreateImage( cvGetSize(gray), IPL_DEPTH_64F, 1);
        IplImage *  realInputDouble = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
        IplImage *  imaginaryInput = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
        IplImage *  complexInput = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 2);
        cvScale(grayfaces[i], realInput, 1.0, 0.0);
        cvZero(realInputDouble);
        cvZero(imaginaryInput);
        cvGetSubRect( realInputDouble, &tmp, cvRect(0,0,SIZE_OF_IMAGE,SIZE_OF_IMAGE));
        cvCopy(realInput,&tmp);
        cvMerge(realInputDouble, imaginaryInput, NULL, NULL, complexInput);

        CvMat * dftImage = cvCreateMat( SIZE_OF_IMAGE_2X, SIZE_OF_IMAGE_2X, CV_64FC2 );
        cvGetSubRect( dftImage, &tmp, cvRect(0,0,SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X));
        cvCopy( complexInput, &tmp, NULL );
        cvDFT( dftImage, dftImage, CV_DXT_FORWARD,0);
        CvScalar s1;
        s1=cvGet2D(dftImage,0,0);
        // printf("%e \n",s1.val[0]);
        int l=0,m=0;
        for (l=0;l<SIZE_OF_IMAGE_2X;l++)
        {
            for (m=0;m<SIZE_OF_IMAGE_2X;m++)
            {

                CvScalar scalar = cvGet2D( dftImage, l, m );
                scalar.val[0]/=s1.val[0];
                scalar.val[1]/=s1.val[0];

                cvSet2D(S,(l*SIZE_OF_IMAGE_2X + m), i,scalar);

                //   if(i==0)
                ////printf("%e %e VAL of S1 and S2 \n",scalar.val[0],scalar.val[1]);

                CvScalar scalarConj;
                scalarConj.val[0]=scalar.val[0];
                scalarConj.val[1]=-scalar.val[1];
                cvSet2D(SPLUS,i,(l*SIZE_OF_IMAGE_2X + m),scalarConj);
                double val=((pow(scalar.val[0],2)+pow(scalar.val[1],2)));

                CvScalar s1= cvGet2D(maxEnergy,(l*SIZE_OF_IMAGE_2X + m),0);
                if (s1.val[0]<val)
                    s1.val[0]=val;
                cvSet2D(maxEnergy,(l*SIZE_OF_IMAGE_2X + m),0,s1);

                CvScalar s= cvGet2D(D,(l*SIZE_OF_IMAGE_2X + m),0);
                s.val[0]=s.val[0]+val;
                s.val[1]=0;
                cvSet2D(  D, (l*SIZE_OF_IMAGE_2X + m), 0,s);

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
        CvScalar s1= cvGet2D(maxEnergy,i,0);
        if (s1.val[0]==0)
            s1.val[0]=1;
        //  //printf("%e %e   VAL of S1 and S2 \n",s.val[0],s.val[1]);

        s.val[0]=(s1.val[0]*size/(s.val[0]));
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
    CvMat *  maceFilterVisualize = cvCreateMat( SIZE_OF_IMAGE_2X, SIZE_OF_IMAGE_2X, CV_64FC2 );
    for (l=0;l<SIZE_OF_IMAGE_2X;l++)
    {
        for (m=0;m<SIZE_OF_IMAGE_2X;m++)
        {
            CvScalar s1= cvGet2D(Hmace_FIN,(l*SIZE_OF_IMAGE_2X +m),0);
            cvSet2D(maceFilterVisualize, l, m,s1);
        }

    }
    //CvScalar s1= cvGet2D(Hmace_FIN,0,0);


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
double peakCorrPlaneEnergy(CvMat*maceFilterVisualize,IplImage *img,int  SIZE_OF_IMAGE)
{
    int  SIZE_OF_IMAGE_2X =SIZE_OF_IMAGE*2;
    int  TOTALPIXEL =SIZE_OF_IMAGE_2X*SIZE_OF_IMAGE_2X;


    IplImage* face=cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    cvCvtColor( img, face, CV_BGR2GRAY );

    IplImage * grayImage= cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE),8,1);
    cvResize(face, grayImage, CV_INTER_LINEAR ) ;
    cvEqualizeHist( grayImage,grayImage);
    IplImage *  realInput = cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE), IPL_DEPTH_64F, 1);
    IplImage *  realInputDouble = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
    IplImage *  imaginaryInput = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
    IplImage *  complexInput = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 2);

    cvScale(grayImage, realInput, 1.0, 0.0);


    cvZero(imaginaryInput);
    cvZero(realInputDouble);

    CvMat tmp;
    cvGetSubRect( realInputDouble, &tmp, cvRect(0,0,SIZE_OF_IMAGE,SIZE_OF_IMAGE));
    cvCopy(realInput,&tmp);
    cvMerge(realInputDouble, imaginaryInput, NULL, NULL, complexInput);
    CvMat * dftImage = cvCreateMat( SIZE_OF_IMAGE_2X, SIZE_OF_IMAGE_2X, CV_64FC2 );
    cvGetSubRect( dftImage, &tmp, cvRect(0,0,SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X));
    cvCopy( complexInput, &tmp, NULL );
    cvDFT( dftImage, dftImage, CV_DXT_FORWARD,0);
    int l=0,m=0;
    /*
     CvScalar s1;
     s1=cvGet2D(dftImage,0,0);
     for (l=0;l<SIZE_OF_IMAGE_2X;l++)
     {
         for (m=0;m<SIZE_OF_IMAGE_2X;m++)
         {

             CvScalar scalar = cvGet2D( dftImage, l, m );
             scalar.val[0]/=s1.val[0];
             scalar.val[1]/=s1.val[0];
             cvSet2D( dftImage, l, m,scalar );
         }
    }
    */
    cvMulSpectrums(dftImage , maceFilterVisualize, dftImage,CV_DXT_MUL_CONJ);
    cvDFT( dftImage, dftImage ,CV_DXT_INV_SCALE,0 );
    IplImage * image_Re = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
    IplImage * image_Im = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
    cvSplit( dftImage, image_Re, image_Im, 0, 0 );
    cvShiftDFT(image_Re,image_Re);
    double m1,M1;
    CvPoint p1,p2;
    cvMinMaxLoc(image_Re, &m1, &M1, &p1, &p2, NULL);
    double valueOfPCER=0;
    for (l=0;l<SIZE_OF_IMAGE_2X;l++)
    {
        for (m=0;m<SIZE_OF_IMAGE_2X;m++)
        {
            CvScalar scalar = cvGet2D( image_Re, l, m );
            valueOfPCER+=scalar.val[0];
        }

    }
    // static int namedw=0;
    //// namedw++;
    // /// char namedwin[300];
    // sprintf(namedwin,"win %d\n",namedw);
    // cvNamedWindow(namedwin,0);
    // //     cvScale(image_Re, image_Re, 1.0/(M1-m1), 1.0*(-m1)/(M1-m1));
    //  printf("%e MAX  %e MIN \n",M1,m1);

    //cvShowImage(namedwin,image_Re);

    cvReleaseImage(&face);
    cvReleaseImage(&grayImage);
    cvReleaseMat(&dftImage);
    cvReleaseImage(&image_Re);
    cvReleaseImage(&image_Im);
    cvReleaseImage(&realInput);
    cvReleaseImage(&realInputDouble);
    cvReleaseImage(&imaginaryInput);
    cvReleaseImage(&complexInput);
    double PCER=0;
    PCER=M1/sqrt(valueOfPCER);
    //printf("%e \n",PCER);
    return PCER;

}

int peakToSideLobeRatio(CvMat*maceFilterVisualize,IplImage *img,int  SIZE_OF_IMAGE)
{
    int  SIZE_OF_IMAGE_2X =SIZE_OF_IMAGE*2;
    int  TOTALPIXEL =SIZE_OF_IMAGE_2X*SIZE_OF_IMAGE_2X;

    int radius1=int(floor((double)(45.0/64.0)*(double)SIZE_OF_IMAGE));
    int radius2=int(floor((double)(27.0/64.0)*(double)SIZE_OF_IMAGE));

    int rad1=radius1;
    int rad2=radius2;
    // printf("%d %d  \n",rad1,rad2);
    IplImage* face=cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    cvCvtColor( img, face, CV_BGR2GRAY );
//logOfImage(face,face);
    //  face=featureLBPSum(face);
    // cvEqualizeHist( face,face);
    // cvEqualizeHist( face,face);
    IplImage * grayImage= cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE),8,1);
    cvResize(face, grayImage, CV_INTER_LINEAR ) ;
    cvEqualizeHist( grayImage,grayImage);
    IplImage *  realInput = cvCreateImage( cvSize(SIZE_OF_IMAGE,SIZE_OF_IMAGE), IPL_DEPTH_64F, 1);
    IplImage *  realInputDouble = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
    IplImage *  imaginaryInput = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
    IplImage *  complexInput = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 2);

    cvScale(grayImage, realInput, 1.0, 0.0);
    cvZero(imaginaryInput);
    cvZero(realInputDouble);

    CvMat tmp;
    cvGetSubRect( realInputDouble, &tmp, cvRect(0,0,SIZE_OF_IMAGE,SIZE_OF_IMAGE));
    cvCopy(realInput,&tmp);
    cvMerge(realInputDouble, imaginaryInput, NULL, NULL, complexInput);

    CvMat * dftImage = cvCreateMat( SIZE_OF_IMAGE_2X, SIZE_OF_IMAGE_2X, CV_64FC2 );
    cvGetSubRect( dftImage, &tmp, cvRect(0,0,SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X));
    cvCopy( complexInput, &tmp, NULL );
    cvDFT( dftImage, dftImage, CV_DXT_FORWARD,0);
    cvMulSpectrums(dftImage , maceFilterVisualize, dftImage,CV_DXT_MUL_CONJ);

    cvDFT( dftImage, dftImage, CV_DXT_INV_SCALE,0 );
    IplImage * image_Re = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);
    IplImage * image_ReV = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);

    IplImage *   image_Im = cvCreateImage( cvSize(SIZE_OF_IMAGE_2X,SIZE_OF_IMAGE_2X), IPL_DEPTH_64F, 1);


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
    static int count=0;
    count++;
    char a[3];
    cvScale(image_Re, image_Re, 1.0, 1.0*(-m1));
    cvMinMaxLoc(image_Re, &m1, &M1, &p1, &p2, NULL);
    int l=0,m=0;
    double value=0;
    double num=0;

    for (l=0;l<SIZE_OF_IMAGE_2X;l++)
    {
        for (m=0;m<SIZE_OF_IMAGE_2X;m++)
        {

            double rad=sqrt((pow(m-SIZE_OF_IMAGE,2)+pow(l-SIZE_OF_IMAGE,2)));
            if (rad<rad1)
            {

                if (rad>rad2)
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
    for (l=0;l<SIZE_OF_IMAGE_2X;l++)
    {
        for (m=0;m<SIZE_OF_IMAGE_2X;m++)
        {
            double rad=sqrt((pow(m-SIZE_OF_IMAGE,2)+pow(l-SIZE_OF_IMAGE,2)));
            if (rad<rad1)
            {

                if (rad>rad2)
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
    CvScalar sca= cvGet2D(image_Re,SIZE_OF_IMAGE,SIZE_OF_IMAGE);
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




