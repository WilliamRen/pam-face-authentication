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

void faceDetect( IplImage* img,CvPoint* p1,CvPoint* p2,CvMemStorage* storage,CvHaarClassifierCascade*  cascade)
{
    int i;

    static CvScalar colors[] = {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}}
    };


    IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    cvCvtColor( img, gray, CV_BGR2GRAY );
    cvEqualizeHist( gray, gray );
    if( cascade )
    {

        CvSeq* faces = cvHaarDetectObjects( gray, cascade, storage,1.1, 2, CV_HAAR_DO_CANNY_PRUNING,cvSize(48,48) );
       for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
             (*p1).x =(r->x);
             (*p1).y= (r->y);
             (*p2).x=(r->x+r->width);
             (*p2).y=(r->y+r->height);
       //     cvRectangle(img,p1,p2,colors[3],3,4,0);

        }
    }


    cvReleaseImage( &gray );

}

void saveFace(IplImage* frame_copy,CvPoint* p1,CvPoint* p2,char* path,char * filename)
{
   IplImage* extract=cvCloneImage(frame_copy);
               cvSetImageROI(extract,cvRect(((*p1).x),((*p1).y),((*p2).x-(*p1).x),((*p2).y-(*p1).y)));
                IplImage* faceDetected=cvCreateImage( cvSize(((*p2).x-(*p1).x),((*p2).y-(*p1).y)), 8, 3 );
                IplImage* faceDetectedGray=cvCreateImage( cvSize(((*p2).x-(*p1).x),((*p2).y-(*p1).y)), 8, 1 );
                cvResize( extract,faceDetected, CV_INTER_LINEAR );
                cvCvtColor(faceDetected, faceDetectedGray, CV_BGR2GRAY );
                cvEqualizeHist(faceDetectedGray, faceDetectedGray);
                IplImage* resizedfaceDetectedGray = cvCreateImage(cvSize(92,112),8,1);
                cvResize(faceDetectedGray,resizedfaceDetectedGray,CV_INTER_LINEAR );

            char* userFile;
            userFile=(char *)calloc(strlen(path) + strlen(filename) + 5,sizeof(char));
            strcat(userFile,path);
            strcat(userFile,filename);
            strcat(userFile,".pgm");
            //printf("%s ",userFile);
            cvSaveImage(userFile,resizedfaceDetectedGray);

              cvReleaseImage( &extract );
            cvReleaseImage( &faceDetected );
            cvReleaseImage( &faceDetectedGray );
            cvReleaseImage( &resizedfaceDetectedGray );
}
