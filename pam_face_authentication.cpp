/*
    Rewritten
    Google Summer of Code Program 2009
    Mentoring Organization: pardus
    Mentor: Onur Kucuk

    Copyright (C) 2008-2009
     Rohan Anil (rohan.anil@gmail.com)
     Alex Lau ( avengermojo@gmail.com)

    Google Summer of Code Program 2008
    Mentoring Organization: openSUSE


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
#include <security/_pam_types.h>
#include <pwd.h> /* getpwdid */
#include "cv.h"
#include "highgui.h"
#include <sys/types.h>
#include <sys/stat.h>
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
#include "pam_face_authentication.h"
#include "pam_face_defines.h"

#include "opencvWebcam.h"
#include "detector.h"
#include "verifier.h"
int file_exists(const char* filename);

char * prevmsg=0;
int msgPipeLiner(char *msg)
{
    if (prevmsg!=0)
    {
        if (strcmp(prevmsg,msg)==0)
            return 0;
    }

    if (prevmsg!=0)
        free(prevmsg);
    prevmsg=(char *)calloc(strlen(msg)+1,sizeof(char));
    strcpy(prevmsg,msg);
    return 1;
}

static int send_info_msg(pam_handle_t *pamh, char *msg)
{
    if (msgPipeLiner(msg)==0)
        return 0;
    struct pam_message mymsg;
    mymsg.msg_style = PAM_TEXT_INFO;
    mymsg.msg = msg;
    const struct pam_message *msgp = &mymsg;
    const struct pam_conv *pc;
    struct pam_response *resp;
    int r;

    r = pam_get_item(pamh, PAM_CONV, (const void **) &pc);
    if (r != PAM_SUCCESS)
        return -1;

    if (!pc || !pc->conv)
        return -1;

    return pc->conv(1, &msgp, &resp, pc->appdata_ptr);
}

static int send_err_msg(pam_handle_t *pamh, char *msg)
{
    if (msgPipeLiner(msg)==0)
        return 0;
    struct pam_message mymsg;
    mymsg.msg_style = PAM_ERROR_MSG;
    mymsg.msg = msg;

    const struct pam_message *msgp = &mymsg;
    const struct pam_conv *pc;
    struct pam_response *resp;
    int r;

    r = pam_get_item(pamh, PAM_CONV, (const void **) &pc);
    if (r != PAM_SUCCESS)
        return -1;

    if (!pc || !pc->conv)
        return -1;

    return pc->conv(1, &msgp, &resp, pc->appdata_ptr);
}


void resetFlags()
{
    *commAuth=0;
}


void ipcStart()
{
    /*   IPC   */
    ipckey =  IPC_KEY_IMAGE;
    shmid = shmget(ipckey, IMAGE_SIZE, IPC_CREAT | 0666);
    shared = (char *)shmat(shmid, NULL, 0);

    ipckeyCommAuth = IPC_KEY_STATUS;
    shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
    commAuth = (int *)shmat(shmidCommAuth, NULL, 0);

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
            int val3=(uchar)s.val[2];
            int val2=(uchar)s.val[1];
            int val1=(uchar)s.val[0];

            *(shared + m*3 + 2+ n*IMAGE_WIDTH*3)=val3;
            *(shared + m*3 + 1+ n*IMAGE_WIDTH*3)=val2;
            *(shared + m*3 + 0+ n*IMAGE_WIDTH*3)=val1;

        }
    }
}
PAM_EXTERN
int pam_sm_authenticate(pam_handle_t *pamh,int flags,int argc
                        ,const char **argv)
{
    printf("Started PAM \n");

    int retval;
    const char *user=NULL;
    const char *error;

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
        send_err_msg(pamh, "Username Not Set.");
        return PAM_AUTHINFO_UNAVAIL;
    }
    int retValMsg;
    /*\m/ \m/  \m/ \m/  \m/ \m/  yay ! removed Xauth stuff Not Needed for KDM or GDM  ! yay \m/ \m/  \m/ \m/  \m/ \m/  */
    ipcStart();
    resetFlags();

    char *username=(char *)calloc(strlen(user)+1,sizeof(char));
    strcpy(username,user);


    struct passwd *userStruct;
    userStruct = getpwnam(username);
    uid_t userID=userStruct->pw_uid;
    verifier* newVerifier=new verifier(userID);
    opencvWebcam webcam;
    detector newDetector;


    /* Clear Shared Memory */

    IplImage *zeroFrame=cvCreateImage( cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),IPL_DEPTH_8U,3);
    cvZero(zeroFrame);
    writeImageToMemory(zeroFrame,shared);

    if (webcam.startCamera()==0)
    {
        //Awesome Graphic Could be put to shared memory over here [TODO]
        send_err_msg(pamh, "Unable to get hold of your webcam. Please check if it is plugged in.");
        return PAM_AUTHINFO_UNAVAIL;
    }

    /* New Logic, run it for some X amount of Seconds and Reply Not Allowed */

// This line might be necessary for GDM , will fix later, right now KDM
// system(QT_FACE_AUTH);
    double t1 = (double)cvGetTickCount();
    double t2=0;
    int loop=1;
    while (loop==1 && t2<25000)
    {
        t2 = (double)cvGetTickCount() - t1;
        t2=t2/((double)cvGetTickFrequency()*1000.0);

        IplImage * queryImage = webcam.queryFrame();
        if (queryImage!=0)
        {
            newDetector.runDetector(queryImage);
            if ( newDetector.checkFaceDetected()==1)
            {

                if (sqrt(pow(newDetector.eyesInformation.LE.x-newDetector.eyesInformation.RE.x,2) + (pow(newDetector.eyesInformation.LE.y-newDetector.eyesInformation.RE.y,2)))>28  && sqrt(pow(newDetector.eyesInformation.LE.x-newDetector.eyesInformation.RE.x,2) + (pow(newDetector.eyesInformation.LE.y-newDetector.eyesInformation.RE.y,2)))<120)
                {
                    if (((newDetector.eyesInformation.LE.x>newDetector.faceInformation.LT.x) && (newDetector.eyesInformation.RE.x<newDetector.faceInformation.RB.x)) && ((newDetector.eyesInformation.LE.y>newDetector.faceInformation.LT.y) && (newDetector.eyesInformation.RE.y>newDetector.faceInformation.LT.y)))
                    {
                        double yvalue=newDetector.eyesInformation.RE.y-newDetector.eyesInformation.LE.y;
                        double xvalue=newDetector.eyesInformation.RE.x-newDetector.eyesInformation.LE.x;
                        double ang= atan(yvalue/xvalue)*(180/CV_PI);

                        if (pow(ang,2)<200)
                        {
                            if ((newDetector.eyesInformation.LE.y<(newDetector.faceInformation.LT.y+(newDetector.faceInformation.RB.y-newDetector.faceInformation.LT.y/2))) && (newDetector.eyesInformation.RE.y<(newDetector.faceInformation.LT.y+(newDetector.faceInformation.RB.y-newDetector.faceInformation.LT.y/2))))
                            {
                                IplImage * im = newDetector.clipFace(queryImage);

                                if (newVerifier->verifyFace(im)==1)
                                {
                                    // cvSaveImage("/home/rohan/new1.jpg",newDetector.clipFace(queryImage));
                                    send_info_msg(pamh, "Verification successful.");
                                    webcam.stopCamera();
                                    return PAM_SUCCESS;
                                }
                                else
                                {
                                    send_info_msg(pamh, "Verification failed. Trying again.");
                                }

                                cvReleaseImage(&im);
                            }
                            else
                            {
                                send_info_msg(pamh, "Your eyes are not detected properly.");
                            }


                        }
                        else
                        {
                            send_info_msg(pamh, "The face is tilted at a greater angle, Cannot perform verification.");

                        }

                        cvLine(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE, cvScalar(0,255,0), 4);
                        writeImageToMemory(queryImage,shared);

                    }
                    else
                    {
                        send_info_msg(pamh, "Trying putting Your face to the center of the frame.");
                    }

                }
                else
                {
                    send_info_msg(pamh, "Your eyes are not detected properly. Keep proper distance with the camera.");
                }
            }
            else
            {
                send_info_msg(pamh, "Unable to Detect Your Face.");
            }
        }
        else
        {
            send_info_msg(pamh, "Unable query image from your webcam.");

        }
        cvReleaseImage(&queryImage);

    }

    send_err_msg(pamh, "Giving Up Face Authentication. Try Again=(.");
    webcam.stopCamera();
    return PAM_AUTHINFO_UNAVAIL;
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

