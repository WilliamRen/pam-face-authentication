/*
    Copyright (C) 2008 Rohan Anil (rohan.anil@gmail.com) , Alex Lau ( avengermojo@gmail.com)

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

#include <X11/Xlib.h>
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
#include <malloc.h>
#include <pthread.h>


#include "pam_face_defines.h"
#include "pam_face.h"
CvPoint pLeftEye,pRightEye;
int authenticateThreadReturn=0;
int threadNumber=0;
int answer=-1;
char *userName;
int percentageRecognition;
int currentUserIdRecognition;
int numberofNo=0;
int numberofYes=0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void *funcRecognition(void )
{
    pthread_mutex_lock( &mutex1 );
    if (recognize(&answer,userName,&percentageRecognition,currentUserIdRecognition)=='y')
    {
        numberofYes++;
        if ((numberofYes>=numberofNo))
            authenticateThreadReturn=1;
    }
    else
    {
        authenticateThreadReturn=0;
        numberofNo++;
    }
    //printf("Thread Complete \n");
    threadNumber=0;
    pthread_mutex_unlock( &mutex1 );
}

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
        numberofNo=0;
        numberofYes=0;
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

char startTracker(int *answer,char* username,int currentUserId)
{
    intialize();
    CvPoint pts[4];
    pts[0]=cvPoint(0,0);
    pts[1]=cvPoint(IMAGE_WIDTH,0);
    pts[2]=cvPoint(IMAGE_WIDTH,20);
    pts[3]=cvPoint(0,20);

    CvFont myFont;
    cvInitFont(&myFont,CV_FONT_HERSHEY_DUPLEX, .5f,.5f,0,1,CV_AA);
    char fullPath[300];
    sprintf(fullPath,"/etc/pam-face-authentication/%s.pgm",username);

    /*
        char * fullPath;
        fullPath=(char *)calloc(  strlen(path) + strlen(username)+strlen(imgExt)+1,sizeof(char));
        strcat(fullPath,path);;
        strcat(fullPath,username);
        strcat(fullPath,imgExt);
    */
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
            int k= faceDetect(frame_copy,&pLeftEye,&pRightEye);
            setFlags();
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
                        if ((threadNumber==0) && (authenticateThreadReturn!=1))
                        {
                            threadNumber=1;
                            cvSaveImage(fullPath,face);
                            pthread_t thread1;
                            pthread_create( &thread1, NULL, &funcRecognition,NULL );

                        }
                        if (authenticateThreadReturn==1)
                        {
                            //removeFile(username);
                            AuthenticateButtonClicked=0;
                            *commAuth=EXIT_GUI;
                            cvZero(frame_copy);
                            writeImageToMemory(frame_copy,shared);
                            cvReleaseImage( &frameNew );
                            cvReleaseImage( &frame_copy );
                            cvReleaseCapture( &capture );
                            return 'y';;
                        }




                        /*
                                if (recognize(answer,username,&percentage,currentUserId)=='y')
                                {

                                }
                        */
                        //    printf("percent %d \n",percentage);
                    }
                    cvReleaseImage( &face );


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
    else
    {
        *commAuth==EXIT_GUI;
    }
    return 'n';
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
 //printf("%s Xauth\n",xauthpath);
    if (xauthpath==NULL)
    {
   //     printf("No Xauth\n",xauthpath);
        // We need to extract the Path where Xauth is stored
        // Following Code Sets Xauthority cookie

        // DISPLAY[1] Contains the value



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



    ipcStart();
    resetFlags();
    intializePaths(user);
    userName=(char *)calloc(strlen(user)+1,sizeof(char));
    strcpy(userName,user);
    removeFile(userName);


    /*
    if (findIndex(userName)==-1)
        return PAM_AUTH_ERR;

    */

    system(GTK_FACE_AUTHENTICATE);



    struct passwd *userpasswd;
    userpasswd = getpwnam(userName);
    currentUserIdRecognition=userpasswd->pw_uid;


    if (startTracker(&answer,userName,currentUserIdRecognition)=='y')
    {
        struct passwd *passwd;

        passwd = getpwuid (answer);

        //printf("%s\n",userName);
        //printf("%s\n",passwd->pw_gecos);

        if (strcmp(passwd->pw_gecos,userName)==0)
        {

            // fprintf(t,"AAAA\n");

            return PAM_SUCCESS;

        }

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

