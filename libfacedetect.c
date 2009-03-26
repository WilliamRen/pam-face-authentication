/*
    Copyright (C) 2008 Rohan Anil (rohan.anil@gmail.com) , Alex Lau ( avengermojo@gmail.com)

    Google Summer of Code Program 2008
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
//#include <gsl/gsl_sort.h>
//#include <gsl/gsl_wavelet.h>
//#include <gsl/gsl_wavelet2d.h>






static CvScalar colors[] =
{
    {{0,0,255}},
    {{0,128,255}},
    {{0,255,255}},
    {{0,255,0}},
    {{255,128,0}},
    {{255,255,0}},
    {{255,0,0}},
    {{255,0,255}}
};




char *HAAR_CASCADE_FACE=PKGDATADIR "/haarcascade.xml";
char *HAAR_CASCADE_EYE=PKGDATADIR "/haarcascade_eye_tree_eyeglasses.xml";
char *HAAR_CASCADE_NOSE=PKGDATADIR "/haarcascade_nose.xml";
char *path;
char *imgPath;
char *imgExt=".pgm";
char *GTK_FACE_AUTHENTICATE=BINDIR "/gtk-facetracker &";
char *XAUTH_EXTRACT_FILE="/etc/pam-face-authentication/xauth.key";
char *XAUTHDISPLAY_EXTRACT_FILE="/etc/pam-face-authentication/display.key";
char *GTK_FACE_MANAGER_KEY="/etc/pam-face-authentication/facemanager/face.key";
char *XML_GTK_BUILDER_FACE_MANAGER=PKGDATADIR "/gtk-facemanager.xml";
char *XML_GTK_BUILDER_FACE_AUTHENTICATE=PKGDATADIR "/gtk-faceauthenticate.xml";





int moveTransparent=0;

int init=0;
static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
static CvHaarClassifierCascade* nested_cascade = 0;
double scale = 1;
int widthEye=0;
int widthFace=0;
/*
int dtdLeftEyeOnce=0,dtdRightEyeOnce=0,dtdNoseOnce=0;
int dtdLeftEye=0,dtdRightEye=0,dtdNose=0;
CvPoint p1Nose,p2Nose,p1LeftEye,p1RightEye;

static CvMemStorage* storageFace;
static CvMemStorage* storageEyeLeft;
static CvMemStorage* storageEyeRight;
static CvMemStorage* storageNose;

static CvHaarClassifierCascade* cascadeFace;
static CvHaarClassifierCascade* cascadeEyeLeft;
static CvHaarClassifierCascade* cascadeEyeRight;
static CvHaarClassifierCascade* cascadeNose;
*/
int faceDetect( IplImage* ,CvPoint *,CvPoint *);
void GetSkinMask(IplImage * src_RGB, IplImage * mask_BW, int erosions, int dilations);
void rotate(double angle, float centreX, float centreY,IplImage * img,IplImage * dstimg);
void rotatePoint(CvPoint* srcP,CvPoint* dstP,float angle);
void preprocessIlluminationWT(IplImage * img);
void preprocessIllumination(IplImage * img);
int preprocess(IplImage * img,CvPoint plefteye,CvPoint prighteye,IplImage * face);
double CenterofMass(IplImage* src,int flagXY);
int CheckImageROI(IplImage* img,double x,double y,double width,double height,double fun);
IplImage * eyeTemplateLeft;
IplImage * eyeTemplateRight;
IplImage * noseTemplate;
#define NFEATURES 4
double faceWidth[NFEATURES];
double faceHeight[NFEATURES];
double facep1[4];
double facep2[4];
double eyeLength=0;

void intialize()
{
    cascade = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_FACE, 0, 0, 0 );
    nested_cascade = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_EYE, 0, 0, 0 );
    storage = cvCreateMemStorage(0);

    /*

    cascadeFace       = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_FACE, 0, 0, 0 );
    cascadeEyeLeft    = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_EYE, 0, 0, 0 );
    cascadeEyeRight   = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_EYE, 0, 0, 0 );
    cascadeNose       = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_NOSE, 0, 0, 0 );

    storageFace       = cvCreateMemStorage(0);
    storageEyeLeft    = cvCreateMemStorage(0);
    storageEyeRight   = cvCreateMemStorage(0);
    storageNose       = cvCreateMemStorage(0);
    */
}

void intializePaths(char * username)
{
    char * constChar= "/etc/pam-face-authentication/";
    char * constChar1= "/facemanager/";
    imgPath=(char *)calloc(  strlen(constChar) + strlen(constChar1) +1,sizeof(char));
    strcat(imgPath,constChar);
    strcat(imgPath,constChar1);
    path=(char *)calloc(  strlen(constChar) +1,sizeof(char));
    strcat(path,constChar);

}
void allocateMemory()
{
    cvClearMemStorage( storage );
    /*
    cvClearMemStorage(storageFace );
    cvClearMemStorage( storageNose );
    cvClearMemStorage( storageEyeLeft );
    cvClearMemStorage( storageEyeRight );
    */
}

void GetSkinMask(IplImage * src_RGB, IplImage * mask_BW, int erosions, int dilations)
{
    CvSize size;
    CvSize sz = cvSize( src_RGB->width & -2, src_RGB->height & -2);
    IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8,
                                   3 ); //create 2 temp-images

    IplImage* src = cvCreateImage(cvGetSize(src_RGB), IPL_DEPTH_8U ,
                                  3);
    cvCopyImage(src_RGB, src);
    IplImage* tmpYCR = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U , 3);
    cvCvtColor(src ,tmpYCR , CV_RGB2YCrCb);


    uchar Y;
    uchar Cr;
    uchar Cb;


    CvPixelPosition8u pos_src;
    CvPixelPosition8u pos_dst;

    int x =0;
    int y =0;

    CV_INIT_PIXEL_POS(pos_src,(unsigned char *) tmpYCR->imageData,tmpYCR->widthStep,cvGetSize(tmpYCR),
                      x,y,
                      tmpYCR->origin);

    CV_INIT_PIXEL_POS(pos_dst,
                      (unsigned char *) mask_BW->imageData,
                      mask_BW->widthStep,
                      cvGetSize(mask_BW),
                      x,y,
                      mask_BW->origin);

    uchar * ptr_src;
    uchar * ptr_dst;


    for ( y=0;y<src-> height; y++)
    {

        for ( x=0; x<src->width; x++)
        {

            ptr_src = CV_MOVE_TO(pos_src,x,y,3);
            ptr_dst = CV_MOVE_TO(pos_dst,x,y,3);

            Y = ptr_src[0];
            Cb= ptr_src[1];
            Cr= ptr_src[2];

            if ( Cr > 128 && Cr < 198 &&
                    Cb + 0.6 * Cr >200 && Cb + 0.6 * Cr <225)
            {
                ptr_dst[0] = 255;
                ptr_dst[1] = 255;
                ptr_dst[2] = 255;
            }
            else
            {
                ptr_dst[0] = 0;
                ptr_dst[1] = 0;
                ptr_dst[2] = 0;
            }


        }
    }

    if (erosions>0) cvErode(mask_BW,mask_BW,0,erosions);
    if (dilations>0) cvDilate(mask_BW,mask_BW,0,dilations);

    cvReleaseImage(&pyr);
    cvReleaseImage(&tmpYCR);
    cvReleaseImage(&src);
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

/*
void preprocessIlluminationWT(IplImage * img)
{
    int m,n;
    IplImage* waveletImageUn = cvCreateImage( cvSize(128,128), 8 ,1 ); //untouched

    IplImage* waveletImage = cvCreateImage( cvSize(128,128), 8 ,1 );
    IplImage* waveletImage64 = cvCreateImage( cvSize(64,64), 8 ,1 );
    cvResize( img, waveletImageUn, CV_INTER_LINEAR );
    cvResize( img, waveletImage, CV_INTER_LINEAR );
    //cvEqualizeHist(waveletImage,waveletImage);
    double data[128*128];
    double dataUn[128*128];
    CvPixelPosition8u pos_srcUn;
    CvPixelPosition8u pos_src;
    CvPixelPosition8u pos_src64;
    CV_INIT_PIXEL_POS(pos_srcUn,(unsigned char *) waveletImageUn->imageData,waveletImageUn->widthStep,cvGetSize(waveletImageUn),0,0,waveletImageUn->origin);

    CV_INIT_PIXEL_POS(pos_src64,(unsigned char *) waveletImage64->imageData,waveletImage64->widthStep,cvGetSize(waveletImage64),0,0,waveletImage64->origin);
    CV_INIT_PIXEL_POS(pos_src,(unsigned char *) waveletImage->imageData,waveletImage->widthStep,cvGetSize(waveletImage),0,0,waveletImage->origin);
    uchar * ptr_src;
    uchar * ptr_src64;
    int x=0;
    int y=0;
    uchar Intensity;

    for ( x=0; x<waveletImage->width; x++)
    {

        for ( y=0;y<waveletImage->height; y++)
        {
            ptr_src = CV_MOVE_TO(pos_src,x,y,1);
            Intensity = ptr_src[0];
            data[y*128 +x]=Intensity;
            ptr_src = CV_MOVE_TO(pos_srcUn,x,y,1);
            Intensity = ptr_src[0];
            dataUn[y*128 +x]=Intensity;


        }

    }



    gsl_wavelet *w;
    gsl_wavelet_workspace *work;
    gsl_wavelet_workspace *work1;
    gsl_wavelet_workspace *work2;
    w = gsl_wavelet_alloc (gsl_wavelet_daubechies, 4);
    work = gsl_wavelet_workspace_alloc (128);
    work1 = gsl_wavelet_workspace_alloc (128);
    work2 = gsl_wavelet_workspace_alloc (128);



    gsl_wavelet2d_nstransform_forward (w,data,128,128,128,work);
    gsl_wavelet2d_nstransform_inverse (w,data,128,64,64,work1);


    gsl_wavelet2d_nstransform_forward (w,dataUn,128,128,128,work);
    gsl_wavelet2d_nstransform_inverse (w,dataUn,128,64,64,work1);

    for ( x=0; x<waveletImage->width; x++)
    {

        for ( y=0;y<waveletImage->height; y++)
        {

            if (x>63 || y>63)
            {
                data[y*128 +x]=dataUn[y*128 +x];
            }
            if (x>63 || y>63)
            {
                data[y*128 +x]=1.3*data[y*128 +x];
            }


        }
    }
    gsl_wavelet2d_nstransform_forward (w,data,128,64,64,work1);
    gsl_wavelet2d_nstransform_inverse (w,data,128,128,128,work);
    double min=30000;
    double max=0;
    double sub;
    double ratio;
    for ( x=0; x<waveletImage->width; x++)
    {
        for ( y=0;y<waveletImage->height; y++)
        {
            if (min>data[y*128 +x])
                min=data[y*128 +x];
            if (max<data[y*128 +x])
                max=data[y*128 +x];
        }
    }
    sub=max-min;
    ratio=255/(max-min);
//printf("%e \n",min);
    for ( x=0; x<waveletImage->width; x++)
    {
        for ( y=0;y<waveletImage->height; y++)
        {
            ptr_src = CV_MOVE_TO(pos_src,x,y,1);
            Intensity=(data[y*128 +x] -min)*ratio;
            ptr_src[0]=Intensity;
            if (data[y*128 +x]>255)
                ptr_src[0]=254;
        }
    }
    cvResize( waveletImage, img, CV_INTER_LINEAR );

}
*/
/*
void preprocessIllumination(IplImage * img)
{
    int m,n;

    IplImage* dftImg8 = cvCreateImage( cvSize(img->width,img->height), 8 ,1 );
    cvResize( img, dftImg8, CV_INTER_LINEAR );
    IplImage* dftImg = cvCreateImage( cvSize(img->width,img->height), IPL_DEPTH_32F ,1 );
    cvCvtScale( dftImg8, dftImg ,1,1);
    cvLog(dftImg,dftImg);
    IplImage* filter = cvCreateImage( cvSize(img->width,img->height), IPL_DEPTH_32F, 1 );
//   CvMat *filter2 = cvCreateMat(img->height, img->width, CV_32FC1);
    int divW=2;
    int divH=2;
    int divMax=2;

    double Xmin = (filter->width/2)/divW;
    double Ymin = (filter->height/2)/divH;

    for (m=0;m<filter->width; m++)
    {
        for (n=0; n<filter->height; n++)
        {
            double dxy = sqrt(pow(((m*divW)%filter->width)-Xmin,2)+pow( ((n*divH)%filter->height)-Ymin,2));
            double H;
            if (dxy==0)
            {
                H=0;
            }
            else
                H = (1/(1+ pow((filter->width/(4*divMax*dxy)),2)));   //high pass
            cvSetReal2D(filter,n , m, H);
            //     cvSetReal2D(filter2,n , m, H);
//

        }
    }


    cvDFT( dftImg, dftImg, CV_DXT_FORWARD,0 );
    // cvFilter2D(dftImg,dftImg,filter2,cvPoint(-1,-1));
    cvMul(dftImg, filter,dftImg,1);
    cvDFT( dftImg, dftImg, CV_DXT_INV_SCALE,0 );
    cvExp(dftImg,dftImg);
    cvCvtScale( dftImg, dftImg8 ,1,1);
    cvResize( dftImg8, img, CV_INTER_LINEAR );
}

*/

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

    cvZero(face);
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


    double width=(prighteye.x-plefteye.x);
    double ratio= 80/width;
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

                //cvAnd(face, faceMask, face,NULL);
                // preprocessIllumination(face);
                // preprocessIlluminationWT(face);
                //  cvEqualizeHist(face,face);
                cvReleaseImage( &resizedImg );
                cvReleaseImage( &gray );
                cvReleaseImage( &rotatedImg );

                return 1;
            }

            // cvReleaseImage( &face );
        }
        cvReleaseImage( &resizedImg );
        cvReleaseImage( &gray );
    }
    cvReleaseImage( &rotatedImg );

}

int CheckImageROI(IplImage* img,double x, double y,double width,double height,double fun)
{
    if (img==NULL)
        return -1;
    if (((img->width)<=0) || ((img->height)<=0))
    {
        return -1;
    }

//printf("\n Width=%e  Height=%e x=%e ,y=%e fun=%e",width,height,x,y,fun);
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
printf("%d Number of Faces",faces->total);
for ( i = 0; i < (faces ? faces->total : 0); i++ )
        {  CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

            CvMat small_img_roi;
            CvSeq* nested_objects;
            CvPoint center;
            CvScalar color = colors[i%8];
            int radius;
            width=r->width*scale;
            center.x = cvRound((r->x + r->width*0.5)*scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
            //cvCircle( img, center, radius, color, 3, 8, 0 );
            cvGetSubRect( small_img, &small_img_roi, *r );
            nested_objects = cvHaarDetectObjects( &small_img_roi, nested_cascade, storage,
                                                  1.1, 2, 0
                                                  //|CV_HAAR_FIND_BIGGEST_OBJECT
                                                  //|CV_HAAR_DO_ROUGH_SEARCH
                                                  //|CV_HAAR_DO_CANNY_PRUNING
                                                  //|CV_HAAR_SCALE_IMAGE
                                                  ,
                                                  cvSize(0, 0) );

printf("%d Number of Eyes",nested_objects->total);
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



    /*
        cvSetErrMode( CV_ErrModeSilent );
        dtdLeftEye=0;
        dtdRightEye=0;
        dtdNose=0;
        IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
        IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width),cvRound (img->height)),8, 1 );
        int i,i1;
        cvCvtColor( img, gray, CV_BGR2GRAY );
        cvResize( gray, small_img, CV_INTER_LINEAR );
        cvEqualizeHist( small_img, small_img );
        CvSeq* faces = cvHaarDetectObjects( small_img,cascadeFace, storageFace,1.1, 7, 0,cvSize(100,100) );
        CvPoint p1;
        CvPoint p2;
        if (faces->total>0)
        {

            int max=0;
            int indexFace=-1;
            for ( i = 0; i < faces->total; i++ )
            {
                CvRect* r1 = (CvRect*)cvGetSeqElem(faces, i );
                if (max>r1->width*r1->height)
                {
                    max=r1->width*r1->height;
                    indexFace=i;
                }
            }


            CvRect* r = (CvRect*)cvGetSeqElem( faces, indexFace );

            p1.x=(r->x);
            p1.y=(r->y);
            p2.x=(r->x+r->width);
            p2.y=(r->y+r->height);
            IplImage* eyeLeftPImage = cvCreateImage( cvSize(r->width*2,r->height),8,1);
            IplImage* eyeRightPImage = cvCreateImage( cvSize(r->width*2,r->height),8,1);
            IplImage* eyeFull = cvCreateImage( cvSize(r->width*4,r->height),8,1);

            double scalex=.25;
            double scaley=.25;

            if (CheckImageROI(small_img,(r->x),(((r->y))+(((r->height))/4)),((r->width)/2),(((r->height)*1)/4),3)==-1) return -1;
            cvSetImageROI(small_img,cvRect((r->x),(((r->y))+(((r->height))/4)),((r->width)/2),(((r->height)*1)/4)));
            cvResize( small_img,eyeLeftPImage, CV_INTER_LINEAR ) ;
            cvResetImageROI(small_img);

            int leftEyeDisplacementX=(r->x);
            int leftEyeDisplacementY=(r->y)+((r->height))/4;
            if (CheckImageROI(small_img,(((r->x)+(r->width)/2)),(((r->y))+(((r->height))/4)),((r->width)/2),(((r->height)*1)/4),4)==-1) return -1;
            cvSetImageROI(small_img,cvRect((((r->x)+(r->width)/2)),(((r->y))+(((r->height))/4)),((r->width)/2),(((r->height)*1)/4)));
            cvResize(small_img,eyeRightPImage, CV_INTER_LINEAR);
            cvResetImageROI(small_img);

            int rightEyeDisplacementX=(r->x)+(r->width)/2;
            int rightEyeDisplacementY=(r->y)+((r->height))/4;

            if (CheckImageROI(small_img,(r->x),(r->y),(r->width),(r->height),5)==-1) return -1;
            cvSetImageROI(small_img,cvRect((r->x),(r->y),(r->width),(r->height)));
            IplImage* noseSearchArea = cvCreateImage( cvSize((r->width),(r->height)),8,1);
            cvResize( small_img,noseSearchArea, CV_INTER_LINEAR ) ;
            cvResetImageROI(small_img);
            CvSeq *   eyel = cvHaarDetectObjects( eyeLeftPImage, cascadeEyeLeft, storageEyeLeft,1.1, 6, CV_HAAR_DO_CANNY_PRUNING,cvSize(22,22) );
            CvSeq *   eyer = cvHaarDetectObjects( eyeRightPImage, cascadeEyeRight, storageEyeRight,1.1,6, CV_HAAR_DO_CANNY_PRUNING,cvSize(22,22) );
            CvSeq *   nose = cvHaarDetectObjects( noseSearchArea, cascadeNose, storageNose,1.1, 6, CV_HAAR_DO_CANNY_PRUNING,cvSize(18,15) );

            if (eyel->total>0)
            {
                int max=0;
                int indexEyeLeft=-1;
                for ( i1 = 0; i1 < (eyel ? eyel->total : 0); i1++ )
                {
                    CvRect* r1 = (CvRect*)cvGetSeqElem( eyel, i1 );
                    if (max>r1->width*r1->height)
                    {
                        max=r1->width*r1->height;
                        indexEyeLeft=i1;
                    }
                }

                dtdLeftEye=1;
                dtdLeftEyeOnce=1;
                CvRect* r1 = (CvRect*)cvGetSeqElem( eyel, indexEyeLeft );
                CvPoint p1 ={r1->x,r1->y};
                CvPoint p2 ={r1->x+r1->width,r1->y+r1->height};
                //printf(" 1 %d %d %d %d \n",(r1->x),(r1->y),(r1->width),(r1->height));
                if (CheckImageROI(eyeLeftPImage,(r1->x),(r1->y),(r1->width),(r1->height),6)==-1) return -1;
                cvSetImageROI(eyeLeftPImage,cvRect((r1->x),(r1->y),(r1->width),(r1->height)));
                IplImage* leftEyeSearchArea = cvCreateImage( cvSize((r1->width),(r1->height)),8,1);
                cvResize( eyeLeftPImage,leftEyeSearchArea, CV_INTER_LINEAR ) ;
                cvResetImageROI(eyeLeftPImage);

                double XL=(r1->x+CenterofMass(leftEyeSearchArea,0))*scalex;
                double YL= (r1->y+CenterofMass(leftEyeSearchArea,1))*scaley;
                p1LeftEye.x= leftEyeDisplacementX+XL;
                p1LeftEye.y= leftEyeDisplacementY +YL;
                faceWidth[2]=r->width;
                faceHeight[2]=r->height;
                eyeTemplateLeft= cvCreateImage( cvSize((r1->width),(r1->height)),8,1);
                cvResize( leftEyeSearchArea,eyeTemplateLeft, CV_INTER_LINEAR ) ;
                cvReleaseImage( &leftEyeSearchArea );
            }
            else if (dtdLeftEyeOnce==1)
            {
                double ratioWidth=(r->width/faceWidth[2]);
                double ratioHeight=(r->height/faceHeight[2]);
                if ((eyeLeftPImage->width-(eyeTemplateLeft->width)*ratioWidth)>=0 && ((eyeLeftPImage->height)-(eyeTemplateLeft->height)*ratioHeight)>=0 &&ratioWidth>0 && ratioHeight>0  &&ratioWidth<4 && ratioHeight<4)
                {
                    IplImage* eyeTemplateLeftResized = cvCreateImage( cvSize((eyeTemplateLeft->width)*ratioWidth,(eyeTemplateLeft->height)*ratioHeight),8,1);
                    cvResize(eyeTemplateLeft,eyeTemplateLeftResized , CV_INTER_LINEAR);
                    IplImage* resultMatch = cvCreateImage( cvSize( 1+ (eyeLeftPImage->width-eyeTemplateLeftResized->width),1+ ((eyeLeftPImage->height)-eyeTemplateLeftResized->height)  ), IPL_DEPTH_32F, 1 );
                    cvMatchTemplate( eyeLeftPImage, eyeTemplateLeftResized, resultMatch, CV_TM_SQDIFF );
                    CvPoint    minloc, maxloc;
                    minloc=cvPoint(0,0);
                    maxloc=cvPoint(0,0);
                    double		minval, maxval;
                    cvMinMaxLoc( resultMatch, &minval, &maxval, &minloc, &maxloc, 0 );
                    //printf(" 2 %d %d %d %d \n",(minloc.x),(minloc.y),(eyeTemplateLeftResized->width),(eyeTemplateLeftResized->height));
                    if (CheckImageROI(eyeLeftPImage,(minloc.x),(minloc.y),(eyeTemplateLeftResized->width),(eyeTemplateLeftResized->height),7)==-1) return -1;
                    cvSetImageROI(eyeLeftPImage,cvRect((minloc.x),(minloc.y),(eyeTemplateLeftResized->width),(eyeTemplateLeftResized->height)));
                    IplImage* leftEyeSearchArea = cvCreateImage( cvSize((eyeTemplateLeftResized->width),(eyeTemplateLeftResized->height)),8,1);
                    cvResize( eyeLeftPImage,leftEyeSearchArea, CV_INTER_LINEAR ) ;
                    cvResetImageROI(eyeLeftPImage);

                    double XL=(minloc.x +CenterofMass(leftEyeSearchArea,0))*scalex;
                    double YL= (minloc.y+CenterofMass(leftEyeSearchArea,1))*scaley;
                    p1LeftEye.x= leftEyeDisplacementX+XL;
                    p1LeftEye.y= leftEyeDisplacementY +YL;

                    cvReleaseImage( &eyeTemplateLeftResized );
                    cvReleaseImage( &resultMatch );
                    cvReleaseImage( &leftEyeSearchArea );
                }
            }

            if (eyer->total>0)
            {
                int max=0;
                int indexEyeRight=-1;
                for ( i1 = 0; i1 < (eyer ? eyer->total : 0); i1++ )
                {
                    CvRect* r1 = (CvRect*)cvGetSeqElem( eyer, i1 );
                    if (max>r1->width*r1->height)
                    {
                        max=r1->width*r1->height;
                        indexEyeRight=i1;
                    }
                }
                CvRect* r1 = (CvRect*)cvGetSeqElem( eyer, indexEyeRight );
                CvPoint p1 ={r1->x,r1->y};
                CvPoint p2 ={r1->x+r1->width,r1->y+r1->height};
                if (eyeRightPImage->width>(r1->x)+(r1->width) && eyeRightPImage->height>(r1->y)+(r1->height))
                {
                    if (CheckImageROI(eyeRightPImage,(r1->x),(r1->y),(r1->width),(r1->height),8)==-1) return -1;
                    cvSetImageROI(eyeRightPImage,cvRect((r1->x),(r1->y),(r1->width),(r1->height)));
                    IplImage* rightEyeSearchArea = cvCreateImage( cvSize((r1->width),(r1->height)),8,1);
                    cvResize( eyeRightPImage,rightEyeSearchArea, CV_INTER_LINEAR ) ;
                    cvResetImageROI(eyeRightPImage);

                    double XR=(r1->x +CenterofMass(rightEyeSearchArea,0))*scalex;
                    double YR= (r1->y+CenterofMass(rightEyeSearchArea,1))*scaley;
                    p1RightEye.x= rightEyeDisplacementX+XR;
                    p1RightEye.y= rightEyeDisplacementY +YR;
                    dtdRightEye=1;
                    dtdRightEyeOnce=1;
                    faceWidth[3]=r->width;
                    faceHeight[3]=r->height;
                    facep1[3]=r->x;
                    facep2[3]=r->y;
                    eyeTemplateRight= cvCreateImage( cvSize((r1->width),(r1->height)),8,1);
                    cvResize( rightEyeSearchArea,eyeTemplateRight, CV_INTER_LINEAR ) ;
                    cvReleaseImage( &rightEyeSearchArea );
                }
            }

            else if (dtdRightEyeOnce==1)
            {
                double ratioWidth=(r->width/faceWidth[3]);
                double ratioHeight=(r->height/faceHeight[3]);
                if (ratioWidth>0 &&  ratioHeight>0 &&ratioWidth<4&&  ratioHeight<4&&(eyeRightPImage->width-(eyeTemplateRight->width)*ratioWidth)>=0 && ((eyeRightPImage->height)-(eyeTemplateRight->height)*ratioHeight)>=0 &&ratioWidth>0 && ratioHeight>0 )
                {
                    IplImage* eyeTemplateRightResized = cvCreateImage( cvSize((eyeTemplateRight->width)*ratioWidth,(eyeTemplateRight->height)*ratioHeight),8,1);
                    cvResize(eyeTemplateRight,eyeTemplateRightResized , CV_INTER_LINEAR);
                    IplImage* resultMatch = cvCreateImage( cvSize( 1+ (eyeRightPImage->width-eyeTemplateRightResized->width),1+ ((eyeRightPImage->height)-eyeTemplateRightResized->height)  ), IPL_DEPTH_32F, 1 );
                    cvMatchTemplate( eyeRightPImage, eyeTemplateRightResized, resultMatch, CV_TM_SQDIFF );
                    CvPoint minloc, maxloc;
                    minloc=cvPoint(0,0);
                    maxloc=cvPoint(0,0);
                    minloc=cvPoint(0,0);
                    maxloc=cvPoint(0,0);
                    double minval, maxval;
                    cvMinMaxLoc( resultMatch, &minval, &maxval, &minloc, &maxloc, 0 );
                    if ((minloc.x)>=0 && (minloc.y)>=0 && (eyeTemplateRightResized->width)<=eyeRightPImage->width && (eyeTemplateRightResized->height)<=eyeRightPImage->height )
                    {
                        if (CheckImageROI(eyeRightPImage,(minloc.x),(minloc.y),(eyeTemplateRightResized->width),(eyeTemplateRightResized->height),9)==-1) return -1;
                        cvSetImageROI(eyeRightPImage,cvRect((minloc.x),(minloc.y),(eyeTemplateRightResized->width),(eyeTemplateRightResized->height)));
                        IplImage* rightEyeSearchArea = cvCreateImage( cvSize((eyeTemplateRightResized->width),(eyeTemplateRightResized->height)),8,1);
                        cvResize( eyeRightPImage,rightEyeSearchArea, CV_INTER_LINEAR ) ;
                        cvResetImageROI(eyeRightPImage);

                        double XR=(minloc.x +CenterofMass(rightEyeSearchArea,0))*scalex;
                        double YR= (minloc.y+CenterofMass(rightEyeSearchArea,1))*scaley;
                        p1RightEye.x= rightEyeDisplacementX+XR;
                        p1RightEye.y= rightEyeDisplacementY +YR;
                        cvReleaseImage( &rightEyeSearchArea );
                    }
                    cvReleaseImage( &eyeTemplateRightResized );
                    cvReleaseImage( &resultMatch );

                }
            }

            if (nose->total>0 && dtdRightEye==1 && dtdLeftEye==1)
            {
                int indexNose=0;
                int areaMax=0;
                for ( i1 = 0; i1 < (nose ? nose->total : 0); i1++ )
                {
                    CvRect* r1 = (CvRect*)cvGetSeqElem( nose, i1 );
                    if ( r1->width*r1->height>areaMax)
                    {
                        areaMax=r1->width*r1->height;
                        indexNose=i1;
                    }
                }
                dtdNose=1;
                dtdNoseOnce=1;
                eyeLength=((p1RightEye.x-p1LeftEye.x)>0)?(p1RightEye.x-p1LeftEye.x):0;
                CvRect* r1 = (CvRect*)cvGetSeqElem( nose, indexNose );
                p1Nose.x=r->x+r1->x;
                p1Nose.y=r->y+r1->y;
                p2Nose.x=(r->x+ r1->x+r1->width);
                p2Nose.y=(r1->y+r->y+r1->height);
                faceWidth[0]=r->width;
                faceHeight[0]=r->height;
                noseTemplate = cvCreateImage( cvSize((p2Nose.x-p1Nose.x),(p2Nose.y-p1Nose.y)),8,1);
                if (CheckImageROI(noseSearchArea,(r1->x),(r1->y),(r1->width),(r1->height),10)==-1) return -1;
                cvSetImageROI(noseSearchArea,cvRect((r1->x),(r1->y),(r1->width),(r1->height)));
                cvResize( noseSearchArea,noseTemplate, CV_INTER_LINEAR ) ;
                cvResetImageROI(noseSearchArea);

            }
            else if (dtdNoseOnce==1 && dtdRightEye==1 && dtdLeftEye==1)
            {
                double eyew=((p1RightEye.x-p1LeftEye.x)>0)?(p1RightEye.x-p1LeftEye.x):0;

                double leftD =((p1LeftEye.x-r->x)>0)? (p1LeftEye.x-r->x) :0;
                double ratioWidth=(eyew/eyeLength);
                double ratioHeight=(eyew/eyeLength);
                if (ratioWidth!=0 && ratioHeight!=0 && ratioWidth<4 && ratioHeight<4  && eyew>0)
                {
                    IplImage* noseTemplateResized = cvCreateImage( cvSize((noseTemplate->width)*ratioWidth,(noseTemplate->height)*ratioHeight),8,1);
                    cvResize(noseTemplate,noseTemplateResized, CV_INTER_LINEAR);
                    IplImage* resultMatch = cvCreateImage( cvSize( 1+ ((p1RightEye.x-p1LeftEye.x)-(noseTemplateResized->width)),1+ ((r->height)*5/8-noseTemplateResized->height)  ), IPL_DEPTH_32F, 1 );
                    IplImage* noseSearchAreaReduced= cvCreateImage( cvSize(eyew,(r->height)*5/8),8,1);
                    if (leftD>0 &&  leftD<noseSearchArea->width &&  (leftD+eyew)<noseSearchArea->width && eyew>0 && eyew<IMAGE_WIDTH)
                    {
                        //printf("6 %d %d %d %d \n",leftD,(r->height)/4,eyew,(r->height)*5/8);
                        if (CheckImageROI(noseSearchArea,leftD,(r->height)/4,eyew,(r->height)*5/8,11)==-1) return -1;
                        cvSetImageROI(noseSearchArea,cvRect(leftD,(r->height)/4,eyew,(r->height)*5/8));
                        cvResize( noseSearchArea,noseSearchAreaReduced, CV_INTER_LINEAR ) ;
                        cvResetImageROI(noseSearchArea);

                        cvMatchTemplate( noseSearchAreaReduced, noseTemplateResized, resultMatch, CV_TM_SQDIFF );
                        CvPoint  minloc, maxloc;
                        minloc=cvPoint(0,0);
                        maxloc=cvPoint(0,0);
                        double minval, maxval;
                        cvMinMaxLoc( resultMatch, &minval, &maxval, &minloc, &maxloc, 0 );
                        p1Nose.x=r->x + (p1LeftEye.x-r->x)+minloc.x;
                        p1Nose.y=r->y +(r->height)/4+ minloc.y ;
                        p2Nose.x= r->x+ (p1LeftEye.x-r->x)+minloc.x + noseTemplateResized->width;
                        p2Nose.y=r->y+(r->height)/4+ minloc.y + noseTemplateResized->height ;
                        cvReleaseImage( &noseTemplateResized );
                        cvReleaseImage( &resultMatch );
                        cvReleaseImage( &noseSearchAreaReduced );
                    }
                }
            }


            if (((p2Nose.y+p1Nose.y)/2 -(p1RightEye.y+p1LeftEye.y)/2) <.3*(p1RightEye.x-p1LeftEye.x))
            {
                p2Nose.y=(p1RightEye.y+p1LeftEye.y)/2+.3*(p1RightEye.x-p1LeftEye.x);
                p1Nose.y=(p1RightEye.y+p1LeftEye.y)/2+.2*(p1RightEye.x-p1LeftEye.x);
            }

            if (((p2Nose.y+p1Nose.y)/2 -(p1RightEye.y+p1LeftEye.y)/2) >.8*(p1RightEye.x-p1LeftEye.x))
            {
                p2Nose.y=(p1RightEye.y+p1LeftEye.y)/2+.3*(p1RightEye.x-p1LeftEye.x);
                p1Nose.y=(p1RightEye.y+p1LeftEye.y)/2+.2*(p1RightEye.x-p1LeftEye.x);
            }
            CvPoint nosePosition=cvPoint((p2Nose.x+p1Nose.x)/2,((p2Nose.y+p1Nose.y)/2));

            if (dtdRightEyeOnce==1 && dtdLeftEyeOnce==1 && dtdNose==0 && ((p1LeftEye.x)+(p1RightEye.x-p1LeftEye.x)/3)>10&& ((p1LeftEye.x)+(2*(p1RightEye.x-p1LeftEye.x))/3)<IMAGE_WIDTH &&(p1LeftEye.y)>10 &&(p1LeftEye.y)<230)
            {
                //    printf("%d %d %d %d \n",(p1LeftEye.x)+(p1RightEye.x-p1LeftEye.x)/3,(p1LeftEye.y)-10,(p1RightEye.x-p1LeftEye.x)/3,20);
                if (CheckImageROI(small_img,(p1LeftEye.x)+(p1RightEye.x-p1LeftEye.x)/3,(p1LeftEye.y)-10,(p1RightEye.x-p1LeftEye.x)/3,20,13)==-1) return -1;
                cvSetImageROI(small_img,cvRect((p1LeftEye.x)+(p1RightEye.x-p1LeftEye.x)/3,(p1LeftEye.y)-10,(p1RightEye.x-p1LeftEye.x)/3,20));
                IplImage* pixelsBtIris = cvCreateImage( cvSize((p1RightEye.x-p1LeftEye.x)/3,20),8,1);
                cvResize( small_img,pixelsBtIris, CV_INTER_LINEAR ) ;
                cvResetImageROI(small_img);

                cvEqualizeHist( pixelsBtIris, pixelsBtIris );
                CvPixelPosition8u pos_src;
                CV_INIT_PIXEL_POS(pos_src,(unsigned char *) pixelsBtIris->imageData,pixelsBtIris->widthStep,cvGetSize(pixelsBtIris),0,0,pixelsBtIris->origin);
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
                to1=pixelsBtIris->width;
                to2=pixelsBtIris->height;

                int max=0;
                int maxIndex=0;
                for ( x=0; x<to1; x++)
                {
                    sumPixels[x]=0;
                    for ( y=0;y<to2; y++)
                    {

                        ptr_src = CV_MOVE_TO(pos_src,x,y,1);
                        Intensity = ptr_src[0];
                        sumPixels[x]+=Intensity;
                    }
                    if (sumPixels[x]>max)
                    {
                        maxIndex=x;
                        max=sumPixels[x];
                    }
                }

                nosePosition.x=(p1LeftEye.x)+(p1RightEye.x-p1LeftEye.x)/3+maxIndex;
                if ((nosePosition.x-(p1LeftEye.x+p1RightEye.x)/2)/(p1RightEye.x-p1LeftEye.x)>.125)
                    nosePosition.x=nosePosition.x + .4*(p1RightEye.x-nosePosition.x);
                else if (((p1LeftEye.x+p1RightEye.x)/2-nosePosition.x)/(p1RightEye.x-p1LeftEye.x)>.125)
                    nosePosition.x=nosePosition.x - .4*(nosePosition.x-p1LeftEye.x);
                cvReleaseImage( &pixelsBtIris );
            }


            int  LINE_WIDTH=1,LINE_TYPE=CV_AA;
            cvLine(img, p1LeftEye, nosePosition ,CV_RGB(255,255,255), LINE_WIDTH,LINE_TYPE ,0);
            cvLine(img, p1RightEye, nosePosition ,CV_RGB(255,255,255), LINE_WIDTH,LINE_TYPE ,0);
            cvLine(img, cvPoint((p1LeftEye.x-3),p1LeftEye.y+2), nosePosition ,CV_RGB(255,255,255), 4,LINE_TYPE ,0);
            cvLine(img, cvPoint((p1RightEye.x+3),p1RightEye.y+2), nosePosition ,CV_RGB(255,255,255), 4,LINE_TYPE ,0);
            CvPoint pts[3];
            pts[0]=p1LeftEye;
            pts[1]=p1RightEye;
            pts[2]=nosePosition;
            CvPoint eyeMid;
            eyeMid.x=((p1LeftEye.x+p1RightEye.x)/2);
            eyeMid.y=((p1LeftEye.y+p1RightEye.y)/2);
            cvFillConvexPoly(img, pts,3,CV_RGB(0,255,0),CV_AA,0 );
            */
    /*
            if ((p1LeftEye.x+p1RightEye.x)/2 >nosePosition.x)
            {
                pts[0]=eyeMid;
                pts[1]=p1LeftEye;
                pts[2]=nosePosition;
                cvFillConvexPoly(img, pts,3,CV_RGB(0,180,0),CV_AA,0 );
            }
            else if ((p1LeftEye.x+p1RightEye.x)/2<nosePosition.x)
            {
                pts[0]=eyeMid;
                pts[1]=p1RightEye;
                pts[2]=nosePosition;
                cvFillConvexPoly(img, pts,3,CV_RGB(0,180,0),CV_AA,0 );
            }
    */

    //int colorNo=8;

    /*
            if (dtdLeftEye==1 && dtdRightEye==1)
            {
                cvCircle(img, p1LeftEye, 2, CV_RGB(0,180,0), 3, LINE_TYPE, 0 );
                cvCircle(img, p1RightEye, 2, CV_RGB(0,180,0), 3, LINE_TYPE, 0 );
                cvCircle(img, p1LeftEye, 4, CV_RGB(0,180,0), 3, LINE_TYPE, 0 );
                cvCircle(img, p1RightEye, 4, CV_RGB(0,180,0), 3, LINE_TYPE, 0 );
                cvCircle(img, p1LeftEye,7, CV_RGB(255,255,255), 3, LINE_TYPE, 0 );
                cvCircle(img, p1RightEye, 7, CV_RGB(255,255,255), 3, LINE_TYPE, 0 );
            }
            else
            {
                cvCircle(img, p1LeftEye, 2, CV_RGB(180,180,0), 3, LINE_TYPE, 0 );
                cvCircle(img, p1RightEye, 2, CV_RGB(180,180,0), 3, LINE_TYPE, 0 );
                cvCircle(img, p1LeftEye, 4, CV_RGB(180,180,0), 3, LINE_TYPE, 0 );
                cvCircle(img, p1RightEye, 4, CV_RGB(180,180,0), 3, LINE_TYPE, 0 );
                cvCircle(img, p1LeftEye,7, CV_RGB(255,255,255), 3, LINE_TYPE, 0 );
                cvCircle(img, p1RightEye, 7, CV_RGB(255,255,255), 3, LINE_TYPE, 0 );
            }
            */
    /* cvReleaseImage( &eyeFull );
     cvReleaseImage( &eyeRightPImage );
     cvReleaseImage( &eyeLeftPImage );
     cvReleaseImage( &noseSearchArea );
    }
    CvPoint pts[4];
    pts[0]=cvPoint(0,200);
    pts[1]=cvPoint(IMAGE_WIDTH,200);
    pts[2]=cvPoint(IMAGE_WIDTH,IMAGE_HEIGHT);
    pts[3]=cvPoint(0,IMAGE_HEIGHT);
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );

    if (dtdLeftEye==1 && dtdRightEye==1)
    {

     CvPixelPosition8u pos_dst;
     int x =0;
     int y =0;
     CV_INIT_PIXEL_POS(pos_dst,
                       (unsigned char *) img->imageData,
                       img->widthStep,
                       cvGetSize(img),
                       x,y,
                       img->origin);

     uchar * ptr_dst;
     double tran=255;
     double k=(p2.y)/2;

     moveTransparent-=6;
     if (moveTransparent<=0)
        {
         moveTransparent=((p2.y)-(p1.y));
         }


     for ( y=p1.y+moveTransparent;y<((p2.y)/2) +moveTransparent ; y++)
     {

         k--;

         for ( x=p1.x; x<(p2.x); x++)
         {
             if (y<=p2.y)
             {
                 ptr_dst = CV_MOVE_TO(pos_dst,x,y,3);
                 tran=(255-ptr_dst[0])<(255-ptr_dst[1])?((255-ptr_dst[0])<(255-ptr_dst[2])?(255-ptr_dst[0]):(255-ptr_dst[2])):((255-ptr_dst[1])<(255-ptr_dst[2])?(255-ptr_dst[1]):(255-ptr_dst[2]));
                 //    printf("%e \n" ,tran);
                 double rat=(k/((p2.y-p1.y)/2));

                 tran=tran-((rat)*tran);

                 ptr_dst[0]=ptr_dst[0]+ (int)((tran*ptr_dst[0])/(ptr_dst[0]+ptr_dst[2]+ptr_dst[1]));

                 ptr_dst[1]=ptr_dst[1]+ (int)((tran*ptr_dst[1])/(ptr_dst[0]+ptr_dst[2]+ptr_dst[1]));

                 ptr_dst[2]=ptr_dst[2]+(int)((tran*ptr_dst[2])/(ptr_dst[0]+ptr_dst[2]+ptr_dst[1]));
             }
         }

     }
     cvRectangle(img,p1,p2,colors[3],3,4,0);

     (*pLeftEye).x=p1LeftEye.x;
     (*pLeftEye).y=p1LeftEye.y;
     (*pRightEye).x=p1RightEye.x;
     (*pRightEye).y=p1RightEye.y;
     return 1;
    }
    else
    {
     return 0;
    }
    */
}
