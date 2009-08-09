/** @file */

/*
    Copyright (C) 2008-2009
     Rohan Anil (rohan.anil@gmail.com)
     Alex Lau ( avengermojo@gmail.com)

    Rewritten
    Google Summer of Code Program 2009
    Mentoring Organization: Pardus
    Mentor: Onur Kucuk

    Google Summer of Code Program 2008
    Mentoring Organization: openSUSE

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
#include "webcamImagePaint.h"
#include "opencvWebcam.h"
#include "detector.h"
#include "verifier.h"

#include     <stdio.h>
#include     <stdlib.h>
#include     <string.h>
#include     <X11/Xlib.h>
#include     <X11/Xutil.h>
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

XImage *CreateTrueColorImage(Display *display, Visual *visual,  char *image, int width, int height,IplImage* img)
{
    int max=IMAGE_WIDTH >IMAGE_HEIGHT?IMAGE_WIDTH:IMAGE_HEIGHT;
    int wh=IMAGE_WIDTH >IMAGE_HEIGHT? 1 : 0;

    int i, j;
    char *image32=( char *)malloc(width*height*4);
    char *p=image32;

    for (j=0; j<height; j++)
    {
        for (i=0; i<width; i++)
        {
            if ((j<IMAGE_HEIGHT) && (i<IMAGE_WIDTH))
            {
                CvScalar s;
                s=cvGet2D(img,j,i);
                int val3=(uchar)s.val[2];
                int val2=(uchar)s.val[1];
                int val1=(uchar)s.val[0];


                *p++=val1; // blue
                *p++=val2; // green
                *p++=val3; // red
            }
            else
            {

                *p++=0; // blue
                *p++=0; // green
                *p++=0; // red

            }


            p++;

        }
    }
    return XCreateImage(display, visual, 24, ZPixmap, 0,( char *) image32, width, height, 32, 0);
}

void processEvent(Display *display, Window window, int width, int height,IplImage* img,int s )
{
    int xoffset=(DisplayWidth(display,s) - IMAGE_WIDTH)/2;
    int yoffset=(DisplayHeight(display,s) - IMAGE_HEIGHT)/2;
    // XMoveWindow( display, window, xoffset, yoffset);
    XImage *ximage;
    Visual *visual=DefaultVisual(display, 0);
    ximage=CreateTrueColorImage(display, visual, 0, width, height,img);
    XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 0, 0, width, height);
    XDestroyImage(ximage);
}


PAM_EXTERN
int pam_sm_authenticate(pam_handle_t *pamh,int flags,int argc
                        ,const char **argv)
{
    int retval;
    const char *user=NULL;
    const char *user_request=NULL;

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
    const char* displayOrig=getenv("DISPLAY");
    char* xauthpathOrig=getenv("XAUTHORITY");

    const char* display=getenv("DISPLAY");
    char* xauthpath=getenv("XAUTHORITY");
    // printf("%s \n",xauthpath);


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


    //cvNamedWindow("src",0);
    //cvShowImage("src",
    pam_get_item(pamh,PAM_TTY,(const void **)(const void*)&pamtty);
    if (pamtty!=NULL&&strlen(pamtty)>0)
    {
        if (pamtty[0]==':')
        {
            if (display==NULL)
            {
                display=pamtty;
                if (displayOrig==NULL)
                {
                    setenv("DISPLAY",display,-1);
                }
            }
        }
    }

    int width=IMAGE_WIDTH, height=IMAGE_HEIGHT;
    int s;
    int enableX=0;
    Display *displayScreen;
    Window window;

    if (argc>0)
    {

        if (strcmp(argv[0],"gdmlegacy")==0)
        {
            sprintf(X_lock,"/tmp/.X%s-lock",strtok((char*)&display[1],"."));
            char str[50];
            xlock=fopen(X_lock,"r");
            fgets(cmdline, 300,xlock);
            fclose(xlock);
            char *word1;
            word1=strtok(cmdline,"  \n");
            sprintf(X_lock,"/proc/%s/cmdline",word1);
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

        if ((strcmp(argv[0],"enableX")==0) || (strcmp(argv[0],"enablex")==0))
        {
            pam_get_item(pamh,PAM_RUSER,(const void **)(const void*)&user_request);


            if (user_request!=NULL)
            {

                struct passwd *pw;
                pw = getpwnam(user_request);
                if (pw!=NULL)
                {

                    char xauthPathString[300];
                    if (xauthpathOrig==NULL)
                    {

                        sprintf(xauthPathString,"%s/.Xauthority",pw->pw_dir);
                        setenv("XAUTHORITY",xauthPathString,-1);
                    }

                }

            }
            displayScreen=XOpenDisplay(NULL);
            if (displayScreen!=NULL)
            {

                s = DefaultScreen(displayScreen);
                int xoffset=(DisplayWidth(displayScreen,s) - IMAGE_WIDTH)/2;
                int yoffset=(DisplayHeight(displayScreen,s) - IMAGE_HEIGHT)/2;

                //       printf("%d  %d\n",xoffset ,yoffset);

                window=XCreateSimpleWindow(displayScreen, RootWindow(displayScreen, s), xoffset,xoffset, width, height, 1, 0, 0);
                //XSelectInput(displayScreen, window, ButtonPressMask|ExposureMask);
                XMapWindow(displayScreen, window);
                XMoveWindow(displayScreen, window, xoffset, yoffset);

                enableX=1;


            }

        }
    }


    opencvWebcam webcam;
    detector newDetector;
    static webcamImagePaint newWebcamImagePaint;

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
    int ind=0;
    char tempM[300];
    *commAuth=STARTED;
    send_info_msg(pamh, "Face Verification Pluggable Authentication Module Started");
    int val=newVerifier->verifyFace(zeroFrame);
    if (val==2)
    {
        send_info_msg(pamh, "Biometrics Model not Generated for the User.");
        loop=0;
    }
//send_info_msg(pamh, "Commencing Face Verification.");

    while (loop==1 && t2<25000)
    {
        t2 = (double)cvGetTickCount() - t1;
        t2=t2/((double)cvGetTickFrequency()*1000.0);

        IplImage * queryImage = webcam.queryFrame();
        if (queryImage!=0)
        {

            newDetector.runDetector(queryImage);


            if (sqrt(pow(newDetector.eyesInformation.LE.x-newDetector.eyesInformation.RE.x,2) + (pow(newDetector.eyesInformation.LE.y-newDetector.eyesInformation.RE.y,2)))>28  && sqrt(pow(newDetector.eyesInformation.LE.x-newDetector.eyesInformation.RE.x,2) + (pow(newDetector.eyesInformation.LE.y-newDetector.eyesInformation.RE.y,2)))<120)
            {

                double yvalue=newDetector.eyesInformation.RE.y-newDetector.eyesInformation.LE.y;
                double xvalue=newDetector.eyesInformation.RE.x-newDetector.eyesInformation.LE.x;
                double ang= atan(yvalue/xvalue)*(180/CV_PI);

                if (pow(ang,2)<200)
                {

                    IplImage * im = newDetector.clipFace(queryImage);
                    send_info_msg(pamh, "Verifying Face ...");
                    if (im!=0)
                    {
                        int val=newVerifier->verifyFace(im);
                        if (val==1)
                        {
                            *commAuth=STOPPED;
                            // cvSaveImage("/home/rohan/new1.jpg",newDetector.clipFace(queryImage));
                            send_info_msg(pamh, "Verification successful.");
                            if (enableX==1)
                            {
                                XDestroyWindow(displayScreen,window);
                                XCloseDisplay(displayScreen);
                            }

                            writeImageToMemory(zeroFrame,shared);
                            webcam.stopCamera();

                            return PAM_SUCCESS;
                        }
                    }


                    cvReleaseImage(&im);

                }
                else
                {
                    send_info_msg(pamh, "Align your face.");

                }



                newWebcamImagePaint.paintCyclops(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE);
                newWebcamImagePaint.paintEllipse(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE);

                //  cvLine(queryImage, newDetector.eyesInformation.LE, newDetector.eyesInformation.RE, cvScalar(0,255,0), 4);
            }
            else
            {
                send_info_msg(pamh, "Keep proper distance with the camera.");
            }


            if (enableX==1)
                processEvent(displayScreen, window, width, height,queryImage,s);



            writeImageToMemory(queryImage,shared);
            cvReleaseImage(&queryImage);
        }
        else
        {
            send_info_msg(pamh, "Unable query image from your webcam.");

        }

    }
    writeImageToMemory(zeroFrame,shared);

    send_err_msg(pamh, "Giving Up Face Authentication. Try Again=(.");
    if (enableX==1)
    {
        XDestroyWindow(displayScreen,window);
        XCloseDisplay(displayScreen);
    }
    *commAuth=STOPPED;
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

