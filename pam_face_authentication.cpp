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
            *(shared + m*3 + 2+ n*IMAGE_WIDTH*3)=(uchar)s.val[2];
            *(shared + m*3 + 1+ n*IMAGE_WIDTH*3)=(uchar)s.val[1];
            *(shared + m*3 + 0+ n*IMAGE_WIDTH*3)=(uchar)s.val[0];

        }
    }
}

PAM_EXTERN
int pam_sm_authenticate(pam_handle_t *pamh,int flags,int argc
                        ,const char **argv)
{

    int retval;
    const char *user=NULL;
    const char *error;

    // From fingerprint GUI project
    // We need the Xauth to fork the GUI
    int j;
    const char *pamtty=NULL;
    char X_lock[300];
    char cmdline[300];
    FILE *xlock;
    int length;
    int procnumber=0;
    const char* display=getenv("DISPLAY");
    char* xauthpath=getenv("XAUTHORITY");
    // Following Code Extracts the Display



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

    pam_get_item(pamh,PAM_TTY,(const void **)(const void*)&pamtty);

//printf("%s Xauth\n",xauthpath);

    if (argc>=1)
    {
        if(strcmp(argv[0],"extract_xauth")==0)
        {
        //     printf("No Xauth\n",xauthpath);
        // We need to extract the Path where Xauth is stored
        // Following Code Sets Xauthority cookie

        // DISPLAY[1] Contains the value

    if (pamtty!=NULL&&strlen(pamtty)>0)
    {
        if (pamtty[0]==':')
        {
            if (display==NULL)
            {
                display=pamtty;
                setenv("DISPLAY",display,-1);
            }
        }
    }

        sprintf(X_lock,"/tmp/.X%s-lock",strtok((char*)&display[1],"."));

        /* if (!file_exists(X_lock))
         {
             return -1;
         }
         */
        char str[50];
        xlock=fopen(X_lock,"r");
        fgets(cmdline, 300,xlock);
        fclose(xlock);


        char *word1;
        word1=strtok(cmdline,"  \n");
        sprintf(X_lock,"/proc/%s/cmdline",word1);


        /*
                if (!file_exists(X_lock))
                {
                    return -1;
                }
        */
        xlock=fopen(X_lock,"r");
        fgets (X_lock , 300 , xlock);
        fclose(xlock);

        for (j=0;j<300;j++)
        {
            if (X_lock[j]=='\0')
                X_lock[j]=' ';

        }


        char *word;
        for (word=strtok(X_lock," ");word!=NULL;word=strtok(NULL," "))
        {
            if (strcmp(word,"-auth")==0)
            {
                xauthpath=strtok(NULL," ");
                break;
            }
        }


        if (file_exists(xauthpath)==1)
        {
            setenv("XAUTHORITY",xauthpath,-1);
        }
    }
    }
    ipcStart();
    resetFlags();

    char *username=(char *)calloc(strlen(user)+1,sizeof(char));
    strcpy(username,user);

    system(QT_FACE_AUTH);

    struct passwd *userStruct;
    userStruct = getpwnam(username);
    uid_t userID=userStruct->pw_uid;

    IplImage *zeroFrame=cvCreateImage( cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),IPL_DEPTH_8U,3);
    cvZero(zeroFrame);

    writeImageToMemory(zeroFrame,shared);
    opencvWebcam webcam;
    detector newDetector;
    verifier* newVerifier=new verifier(userID);
    if (webcam.startCamera()==0)
        return PAM_AUTH_ERR;
    else
        *commAuth=STARTED;

    int loop=1;
    while (loop==1 || *commAuth!=CANCEL)
    {
        IplImage * queryImage = webcam.queryFrame();
        newDetector.runDetector(queryImage);

        if (*commAuth==AUTHENTICATE)
        {
          // printf("True Auth \n");
            if (sqrt(pow(newDetector.eyesInformation.LE.x-newDetector.eyesInformation.RE.x,2) + (pow(newDetector.eyesInformation.LE.y-newDetector.eyesInformation.RE.y,2)))>60  && sqrt(pow(newDetector.eyesInformation.LE.x-newDetector.eyesInformation.RE.x,2) + (pow(newDetector.eyesInformation.LE.y-newDetector.eyesInformation.RE.y,2)))<120)
            {
//printf("True lenth \n");
                if (((newDetector.eyesInformation.LE.x>newDetector.faceInformation.LT.x) && (newDetector.eyesInformation.RE.x<newDetector.faceInformation.RB.x)) && ((newDetector.eyesInformation.LE.y>newDetector.faceInformation.LT.y) && (newDetector.eyesInformation.RE.y>newDetector.faceInformation.LT.y)))
                {
                   // printf("True Inside \n");
                    double yvalue=newDetector.eyesInformation.RE.y-newDetector.eyesInformation.LE.y;
                    double xvalue=newDetector.eyesInformation.RE.x-newDetector.eyesInformation.LE.x;
                    double ang= atan(yvalue/xvalue)*(180/CV_PI);

                    if (pow(ang,2)<200)
                    {
                        //printf("True ang \n");
                        if ((newDetector.eyesInformation.LE.y<(newDetector.faceInformation.LT.y+(newDetector.faceInformation.RB.y-newDetector.faceInformation.LT.y/2))) && (newDetector.eyesInformation.RE.y<(newDetector.faceInformation.LT.y+(newDetector.faceInformation.RB.y-newDetector.faceInformation.LT.y/2))))
                        {
                            //printf("True up \n");
                            //printf("True Eye");
                            if (newVerifier->verifyFace(newDetector.clipFace(queryImage))==1)
                            {
                               // cvSaveImage("/home/darksid3hack0r/darksid3.jpg",newDetector.clipFace(queryImage));
                                *commAuth=STOPPED;
                                return PAM_SUCCESS;

                            }
                        }
                    }



                }
            }

        }

        if (*commAuth==CANCEL)
            loop=0;
        cvLine(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE, cvScalar(0,255,0), 4);
        writeImageToMemory(queryImage,shared);

    }


//   return PAM_SUCCESS;

    return PAM_AUTH_ERR;
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

