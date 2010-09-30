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
#define DEFAULT_USER "nobody"
#define PAM_SM_AUTH
#define PAM_SM_ACCOUNT
#define PAM_SM_SESSION
#define PAM_SM_PASSWORD
#include <security/pam_modules.h>
#include <security/_pam_macros.h>
#include <dlfcn.h>
#include "cv.h"
#include "highgui.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
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
#include "pam_face.h"

CvMemStorage* storageFace;
CvMemStorage* storageEyeLeft;
CvMemStorage* storageEyeRight;
CvMemStorage* storageNose;

CvHaarClassifierCascade* cascadeFace;
CvHaarClassifierCascade* cascadeEyeLeft;
CvHaarClassifierCascade* cascadeEyeRight;
CvHaarClassifierCascade* cascadeNose;
CvPoint pLeftEye,pRightEye;
/*
Function Prototypes
of libfacedetect.so

faceDetect(Input Image,EyePoint1,EyePoint2,storage...,..,Cascades...)
returns -1 if unsuccessful


preprocess(Input Image,EyePoint1,EyePoint2,storage...,..,Cascades...);
returns 1 if successful

*/
void *handleFaceDetect;
int (*faceDetect)( IplImage* ,CvPoint *,CvPoint *,CvMemStorage* ,CvMemStorage* ,CvMemStorage* ,CvMemStorage* ,CvHaarClassifierCascade* ,CvHaarClassifierCascade* ,CvHaarClassifierCascade* ,CvHaarClassifierCascade* );
int (*preprocess)( IplImage* ,CvPoint ,CvPoint, IplImage* );

void setFlags()
{
    if ((*commAuth)==0)
    {
        CancelButtonClicked=0;
        AuthenticateButtonClicked=0;
        displayErrorFlag=0;
    }
    if ((*commAuth)==CANCEL)
    {
        CancelButtonClicked=1;
    }
    if ((*commAuth)==AUTHENTICATE)
    {
        AuthenticateButtonClicked=1;
    }
    if ((*commAuth)==DISPLAY_ERROR)
    {
        displayErrorFlag=1;
    }
}

void resetFlags()
{
    *commAuth=0;
    CancelButtonClicked=0;
    AuthenticateButtonClicked=0;
    displayErrorFlag=0;
}

void ipcStart()
{
    /*   IPC   */
    ipckey =  IPC_KEY_IMAGE;
    shmid = shmget(ipckey, IMAGE_SIZE, IPC_CREAT | 0666);
    shared = shmat(shmid, NULL, 0);

    ipckeyCommAuth = IPC_KEY_STATUS;
    shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
    commAuth = shmat(shmidCommAuth, NULL, 0);

    *commAuth=0;
    /*   IPC END  */
}

void writeImageToMemory(IplImage* img,char *shared)
{
    int m,n;
    for (n=0;n<IMAGE_HEIGHT;n++)
    {
        for (m= 0;m<IMAGE_WIDTH;m++)
        {
            CvScalar s;
            s=cvGet2D(img,n,m);
            *(shared + m*3 + 0+ n*IMAGE_WIDTH*3)=(uchar)s.val[0];
            *(shared + m*3 + 1+ n*IMAGE_WIDTH*3)=(uchar)s.val[1];
            *(shared + m*3 + 2+ n*IMAGE_WIDTH*3)=(uchar)s.val[2];
        }
    }
}

void removeFile(char* userTemp)
{
    char* userFile;
    userFile=(char *)calloc(strlen(userTemp) +strlen(path)+strlen(imgExt)+1,sizeof(char));
    strcat(userFile,path);;
    strcat(userFile,userTemp);
    strcat(userFile,imgExt);
    remove(userFile);
}

char startTracker(char* username)
{
    dlerror();
    handleFaceDetect = dlopen(FACE_DETECT_LIBRARY, RTLD_NOW);
    //  printf("ERROR %s \n ",dlerror());
    faceDetect = dlsym(handleFaceDetect, "faceDetect");
    //  printf("ERROR %s \n ",dlerror());
    preprocess = dlsym(handleFaceDetect, "preprocess");
    //  printf("ERROR %s \n ",dlerror());
    intialize();
    CvPoint pts[4];
    pts[0]=cvPoint(0,0);
    pts[1]=cvPoint(IMAGE_WIDTH,0);
    pts[2]=cvPoint(IMAGE_WIDTH,20);
    pts[3]=cvPoint(0,20);
    int percentage;
    CvFont myFont;
    cvInitFont(&myFont,CV_FONT_HERSHEY_DUPLEX, .5f,.5f,0,1,CV_AA);
    void *handleAuthenticate;
    char (*recognize)(char*,int *);
    char * fullPath;
    fullPath=(char *)calloc(  strlen(path) + strlen(username)+strlen(imgExt)+1,sizeof(char));
    strcat(fullPath,path);;
    strcat(fullPath,username);
    strcat(fullPath,imgExt);
    handleAuthenticate = dlopen (FACE_AUTHENTICATE_LIBRARY, RTLD_NOW);
    recognize = dlsym(handleAuthenticate, "recognize");

    CvCapture* capture = 0;
    IplImage *frame,*frameNew,*frame_copy = 0;
    capture = cvCaptureFromCAM(0);
    if ( capture )
    {
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,IMAGE_WIDTH);
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,IMAGE_HEIGHT);
        for (;;)
        {
            if ( !cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture );
            if ( !frame )
                break;
            if ( !frame_copy )
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height),IPL_DEPTH_8U, frame->nChannels );

            if ( frame->origin == IPL_ORIGIN_TL )
                cvCopy( frame, frame_copy, 0 );
            else
                cvFlip( frame, frame_copy, 0 );
            frameNew = cvCreateImage( cvSize(frame->width,frame->height),IPL_DEPTH_8U, frame->nChannels );
            cvCopy(frame_copy, frameNew, 0);
            allocateMemory();
            int k= faceDetect(frame_copy,&pLeftEye,&pRightEye,storageFace,storageEyeLeft,storageEyeRight, storageNose, cascadeFace,cascadeEyeLeft,cascadeEyeRight,cascadeNose);
            setFlags();
          //  cvFillConvexPoly(frame_copy, pts,4,CV_RGB(0,140,0),CV_AA,0 );
           // cvPutText(frame_copy,"PERCEPTRON - :) :D  :P", cvPoint(50,15),&myFont, CV_RGB(255,255,255));

            if (AuthenticateButtonClicked==1)
            {

                if (k==1)
                {
                    IplImage *face;
                    face = cvCreateImage( cvSize(120,140),8,1);
                    int j=0;
                    if (pLeftEye.x>0 && pLeftEye.y>0  && pLeftEye.x<320 && pLeftEye.y<240 && pRightEye.x>0 && pRightEye.y>0&& pRightEye.x<320 && pRightEye.y<240)
                        j=preprocess(frameNew,pLeftEye,pRightEye,face);
                    if (j==1)
                    {
                        cvSaveImage(fullPath,face);

                        if (recognize(username,&percentage)=='y')
                        {
                            removeFile(username);
                            AuthenticateButtonClicked=0;
                            *commAuth=EXIT_GUI;
                            cvZero(frame_copy);
                            writeImageToMemory(frame_copy,shared);
                            cvReleaseImage( &frameNew );
                            cvReleaseImage( &frame_copy );
                            cvReleaseCapture( &capture );
                            return 'y';
                        }
                    //    printf("percent %d \n",percentage);
                    }

                    cvReleaseImage( &face );


                }
                else
                {
//                    cvFillConvexPoly(frame_copy, pts,4,CV_RGB(0,140,0),CV_AA,0 );
  //                  cvPutText(frame_copy,"NO PROPER DETECTION!", cvPoint(26,15),&myFont, CV_RGB(255,255,255));

                }
               cvFillConvexPoly(frame_copy, pts,4,CV_RGB(0,0,0),CV_AA,0 );
              cvPutText(frame_copy,"AUTHENTICATING....", cvPoint(70,15),&myFont, CV_RGB(255,255,255));

           }
            if (CancelButtonClicked==1)
            {
                CancelButtonClicked=0;
                *commAuth=0;
                cvZero(frame_copy);
                writeImageToMemory(frame_copy,shared);
                cvReleaseImage( &frame_copy );
                cvReleaseCapture( &capture );
                return 'n';
            }
            writeImageToMemory(frame_copy,shared);
            cvReleaseImage( &frameNew );
            cvReleaseImage( &frame_copy );

        }
        cvReleaseImage( &frameNew );
        cvReleaseImage( &frame_copy );
        cvReleaseCapture( &capture );
    }

    return 'n';
}
void intialize()
{
    cascadeFace       = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_FACE, 0, 0, 0 );
    cascadeEyeLeft    = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_EYE, 0, 0, 0 );
    cascadeEyeRight   = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_EYE, 0, 0, 0 );
    cascadeNose       = (CvHaarClassifierCascade*)cvLoad( HAAR_CASCADE_NOSE, 0, 0, 0 );
    storageFace       = cvCreateMemStorage(0);
    storageEyeLeft    = cvCreateMemStorage(0);
    storageEyeRight   = cvCreateMemStorage(0);
    storageNose       = cvCreateMemStorage(0);
}

void allocateMemory()
{
    cvClearMemStorage(storageFace );
    cvClearMemStorage( storageNose );
    cvClearMemStorage( storageEyeLeft );
    cvClearMemStorage( storageEyeRight );
}

PAM_EXTERN
int pam_sm_authenticate(pam_handle_t *pamh,int flags,int argc
                        ,const char **argv)
{
    int retval;
    const char *user=NULL;
    const char *error;
    char *userName;

    retval = pam_get_user(pamh, &user, NULL);
    if (retval != PAM_SUCCESS)
    {
        D(("get user returned error: %s", pam_strerror(pamh,retval)));
        return retval;
    }
    if (user == NULL || *user == '\0')
    {
        D(("username not known"));
        pam_set_item(pamh, PAM_USER, (const void *) DEFAULT_USER);
    }
    ipcStart();
    resetFlags();

    system(GTK_FACE_AUTHENTICATE);

    userName=(char *)calloc(strlen(user)+1,sizeof(char));
    strcpy(userName,user);
    removeFile(userName);


    if (startTracker(userName)=='y')
    {
        return PAM_SUCCESS;
    }
    else
    {
        return PAM_AUTH_ERR;
    }

}

PAM_EXTERN
int pam_sm_setcred(pam_handle_t *pamh,int flags,int argc
                   ,const char **argv)
{
    return PAM_SUCCESS;
}

/* --- account management functions --- */

PAM_EXTERN
int pam_sm_acct_mgmt(pam_handle_t *pamh,int flags,int argc
                     ,const char **argv)
{
    return PAM_SUCCESS;
}

/* --- password management --- */

PAM_EXTERN
int pam_sm_chauthtok(pam_handle_t *pamh,int flags,int argc
                     ,const char **argv)
{
    return PAM_SUCCESS;
}

/* --- session management --- */

PAM_EXTERN
int pam_sm_open_session(pam_handle_t *pamh,int flags,int argc
                        ,const char **argv)
{
    return PAM_SUCCESS;
}

PAM_EXTERN
int pam_sm_close_session(pam_handle_t *pamh,int flags,int argc
                         ,const char **argv)
{
    return PAM_SUCCESS;
}


