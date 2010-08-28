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

// PAM and system headers
#include <security/pam_modules.h>
#include <security/_pam_macros.h>
#include <security/_pam_types.h>
#include <pwd.h> /* getpwdid */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

// OpenCV headers
#include "cv.h"
#include "highgui.h"

// App related headers
#include <stdio.h>
#include <libintl.h> // gettext()
#include <X11/Xutil.h> // XDestroyImage()
/*#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <locale.h>
#include <X11/Xlib.h>*/
#include "pam_face_authentication.h"
#include "pam_face_defines.h"
#include "webcamImagePaint.h"
#include "opencvWebcam.h"
#include "detector.h"
#include "verifier.h"

//------------------------------------------------------------------------------
bool msgPipeLiner(char *msg)
{
    if(prevmsg != 0 && strcmp(prevmsg, msg) == 0) return 0;
    if(prevmsg != 0) free(prevmsg);
    
    prevmsg = (char *)calloc(strlen(msg)+1, sizeof(char));
    strcpy(prevmsg, msg);
    
    return 1;
}

//------------------------------------------------------------------------------
static int send_msg(pam_handle_t* pamh, char* msg, int type)
{
    const struct pam_conv* pc;
    struct pam_message mymsg;
    struct pam_response* resp;
    
    if(boolMessages == false) return 0;
    if(msgPipeLiner(msg) == false) return 0;

    if(type == 1) mymsg.msg_style = PAM_ERROR_MSG;  // print error msg
    else mymsg.msg_style = PAM_TEXT_INFO;  // normal output
    mymsg.msg = msg;
    const struct pam_message* msgp = &mymsg;

    int r = pam_get_item(pamh, PAM_CONV, (const void **)&pc);
    if(r != PAM_SUCCESS) return -1;

    if(!pc || !pc->conv) return -1;

    return pc->conv(1, &msgp, &resp, pc->appdata_ptr);
}

//------------------------------------------------------------------------------
void resetFlags()
{
    *commAuth = 0;
}

//------------------------------------------------------------------------------
void ipcStart()
{
    /*   IPC   */
    ipckey = IPC_KEY_IMAGE;
    shmid = shmget(ipckey, IMAGE_SIZE, IPC_CREAT | 0666);
    shared = (char *)shmat(shmid, NULL, 0);

    ipckeyCommAuth = IPC_KEY_STATUS;
    shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
    commAuth = (int *)shmat(shmidCommAuth, NULL, 0);

    *commAuth = 0;
    /*   IPC END  */
}

//------------------------------------------------------------------------------
void writeImageToMemory(IplImage* img, char* shared)
{
    for(int n = 0; n < IMAGE_HEIGHT; n++)
    {
        for(int m = 0; m < IMAGE_WIDTH; m++)
        {
            CvScalar s = cvGet2D(img, n, m);
            int val3 = (uchar)s.val[2];
            int val2 = (uchar)s.val[1];
            int val1 = (uchar)s.val[0];

            *(shared + m*3 + 2+ n*IMAGE_WIDTH*3) = val3;
            *(shared + m*3 + 1+ n*IMAGE_WIDTH*3) = val2;
            *(shared + m*3 + 0+ n*IMAGE_WIDTH*3) = val1;
        }
    }
}

//------------------------------------------------------------------------------
XImage* CreateTrueColorImage(Display* display, Visual* visual, 
  char* image, int width, int height, IplImage* img)
{
    int max = (IMAGE_WIDTH > IMAGE_HEIGHT) ? IMAGE_WIDTH:IMAGE_HEIGHT;
    int wh = (IMAGE_WIDTH > IMAGE_HEIGHT) ? 1 : 0;

    char* image32=(char *)malloc(width*height*4);
    char* p = image32;

    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            if((j < IMAGE_HEIGHT) && (i < IMAGE_WIDTH))
            {
                CvScalar s = cvGet2D(img, j, i);
                int val3 = (uchar)s.val[2];
                int val2 = (uchar)s.val[1];
                int val1 = (uchar)s.val[0];

                *p++ = val1; // blue
                *p++ = val2; // green
                *p++ = val3; // red
            }
            else
            {
                *p++ = 0; // blue
                *p++ = 0; // green
                *p++ = 0; // red
            }
        p++;
        }
    }
    
    return XCreateImage(display, visual, 24, ZPixmap, 0, 
      (char *)image32, width, height, 32, 0);
}

//------------------------------------------------------------------------------
void processEvent(Display* display, Window window, 
  int width, int height, IplImage* img, int s)
{
    int xoffset = (DisplayWidth(display, s) - IMAGE_WIDTH)/2;
    int yoffset = (DisplayHeight(display, s) - IMAGE_HEIGHT)/2;
    // XMoveWindow(display, window, xoffset, yoffset);
    
    Visual* visual = DefaultVisual(display, 0);
    XImage* ximage = CreateTrueColorImage(display, visual, 0, width, height, img);
    XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 0, 0, width, height);
    XDestroyImage(ximage);
}

//------------------------------------------------------------------------------
PAM_EXTERN
int pam_sm_authenticate(pam_handle_t* pamh, int flags, int argc, const char** argv)
{
    setlocale(LC_ALL, "");
    bindtextdomain("pam_face_authentication", PKGDATADIR "/locale");
    textdomain("pam_face_authentication");

    Display* displayScreen;
    FILE* xlock = NULL;
    Window window;
    struct passwd* userStruct;

    int k = 0, s, retval, retValMsg, procnumber = 0, length = 0, enableX = 0;
    int width = IMAGE_WIDTH, height = IMAGE_HEIGHT;
    const char* host = NULL;
    const char* pamtty = NULL;
    const char* user = NULL;
    const char* user_request = NULL;
    const char* error = NULL;
    char* username = NULL;

    const char* display = getenv("DISPLAY");
    const char* displayOrig = getenv("DISPLAY");
    char* xauthpath = getenv("XAUTHORITY");
    char* xauthpathOrig = getenv("XAUTHORITY");
    char X_lock[300], cmdline[300];

    // The following lines make sure that the program quits if it's called remotely
    retval = pam_get_item(pamh, PAM_RHOST, (const void**)&host);
    if(host != NULL && host != "localhost") return retval;

    // Fetch the current user name
    retval = pam_get_user(pamh, &user, NULL);
    if(retval != PAM_SUCCESS)
    {
        D(("pam_get_user returned error: %s", pam_strerror(pamh,retval)));
        return retval;
    }
    if (user == NULL || *user == '\0')
    {
        D(("username not known"));
        pam_set_item(pamh, PAM_USER, (const void *) DEFAULT_USER);
        send_msg(pamh, (char*)"Username not set.", 1);
        return PAM_AUTHINFO_UNAVAIL;
    }
    
    /* removed Xauth stuff Not Needed for KDM or GDM  ! yay \m/ \m/  \m/ \m/  \m/ \m/  */
    ipcStart();
    resetFlags();

    username = (char *)calloc(strlen(user)+1, sizeof(char));
    strcpy(username,user);

    userStruct = getpwnam(username);
    uid_t userID = userStruct->pw_uid;
    verifier* newVerifier = new verifier(userID);

    pam_get_item(pamh, PAM_TTY, (const void **)(const void*)&pamtty);
    if(pamtty != NULL && strlen(pamtty) > 0 && pamtty[0] == ':' && display == NULL)
    {
        display = pamtty;
        if(displayOrig == NULL) setenv("DISPLAY", display, -1);
    }

    while(k < argc)
    {
        if(strcmp(argv[k], "gdmlegacy") == 0)
        {
            char str[50];
            char* word = NULL;
            char* word1 = NULL;
            
            sprintf(X_lock, "/tmp/.X%s-lock", strtok((char*)&display[1], "."));
            
            xlock = fopen(X_lock, "r");
            fgets(cmdline, 300, xlock);
            fclose(xlock);
            
            word1 = strtok(cmdline,"  \n");
            sprintf(X_lock, "/proc/%s/cmdline", word1);
            xlock = fopen(X_lock, "r");
            fgets(X_lock , 300, xlock);
            fclose(xlock);
            
            for(int j = 0; j < 300; j++)
            {
                if (X_lock[j]=='\0') X_lock[j]=' ';
            }
            
            for(word = strtok(X_lock, " "); word != NULL; word = strtok(NULL," "))
            {
                if(strcmp(word, "-auth") == 0)
                {
                    xauthpath = strtok(NULL, " ");
                    break;
                }
            }
            if(file_exists(xauthpath) == 1) setenv("XAUTHORITY", xauthpath, -1);
        }

        if((strcmp(argv[k], "enableX") == 0) || (strcmp(argv[k], "enablex") == 0))
        {
            pam_get_item(pamh, PAM_RUSER, (const void **)(const void*)&user_request);

            if(user_request != NULL)
            {
                struct passwd* pw = getpwnam(user_request);
                if(pw != NULL && xauthpathOrig == NULL)
                {  
                    char xauthPathString[300];
                    sprintf(xauthPathString, "%s/.Xauthority", pw->pw_dir);
                    setenv("XAUTHORITY",xauthPathString,-1);
                }
            }

            displayScreen = XOpenDisplay(NULL);
            if(displayScreen != NULL)
            {
                s = DefaultScreen(displayScreen);
                int xoffset = (DisplayWidth(displayScreen,s) - IMAGE_WIDTH)/2;
                int yoffset = (DisplayHeight(displayScreen,s) - IMAGE_HEIGHT)/2;

                // printf("%d  %d\n",xoffset ,yoffset);

                window = XCreateSimpleWindow(displayScreen, 
                    RootWindow(displayScreen, s), xoffset, xoffset, width, height, 1, 0, 0);
                //XSelectInput(displayScreen, window, ButtonPressMask|ExposureMask);
                XMapWindow(displayScreen, window);
                XMoveWindow(displayScreen, window, xoffset, yoffset);

                enableX = 1;
            }
        }

        if(strcmp(argv[k], "disable-messages") == 0) boolMessages = false;

    k++;
    }

    opencvWebcam webcam;
    detector newDetector;
    static webcamImagePaint newWebcamImagePaint;

    /* Clear Shared Memory */
    IplImage* zeroFrame = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), IPL_DEPTH_8U, 3);
    cvZero(zeroFrame);
    writeImageToMemory(zeroFrame, shared);

    if(webcam.startCamera() == 0)
    {
        //Awesome Graphic Could be put to shared memory over here [TODO]
        send_msg(pamh, gettext("Unable to get hold of your webcam. Please check if it is plugged in."), 1);
        return PAM_AUTHINFO_UNAVAIL;
    }

    /* New Logic, run it for some X amount of Seconds and Reply Not Allowed */

    // This line might be necessary for GDM , will fix later, right now KDM
    // system(QT_FACE_AUTH);
    double t1 = (double)cvGetTickCount();
    double t2 = 0;
    double t3 = 0;
    bool run_loop = true;
    int timeout = 25000; // Login timeout value
    
    *commAuth = STARTED;
    
    // Don't Gettext this because kgreet_plugin relies on this :)
    send_msg(pamh, (char*)"Face Verification Pluggable Authentication Module Started");
    int val = newVerifier->verifyFace(zeroFrame);
    if(val == 2)
    {
        send_msg(pamh, gettext("Biometrics model has not been generated for the user. \
          Use qt-facetrainer to create the model."));
        run_loop = false;
    }
    
    //send_msg(pamh, "Commencing Face Verification.");
    
    CvFileStorage* fileStorage = cvOpenFileStorage(PKGDATADIR "/config.xml", 0, CV_STORAGE_READ);
    if(fileStorage)
    {
        timeout = cvReadIntByName(fileStorage, 0, "TIME_OUT", timeout);
        cvReleaseFileStorage(&fileStorage);
    }
    
    while(run_loop == true && t2 < timeout)
    {
        t2 = (double)cvGetTickCount() - t1;
        t2 = t2 / ((double)cvGetTickFrequency()*1000.0);

        IplImage* queryImage = webcam.queryFrame();

        if(queryImage != 0)
        {
            newDetector.runDetector(queryImage);

            if(sqrt(pow(newDetector.eyesInformation.LE.x - newDetector.eyesInformation.RE.x, 2) 
              + (pow(newDetector.eyesInformation.LE.y-newDetector.eyesInformation.RE.y, 2))) > 28  
            && sqrt(pow(newDetector.eyesInformation.LE.x-newDetector.eyesInformation.RE.x, 2) 
              + (pow(newDetector.eyesInformation.LE.y-newDetector.eyesInformation.RE.y, 2))) < 120)
            {
                double yvalue = newDetector.eyesInformation.RE.y - newDetector.eyesInformation.LE.y;
                double xvalue = newDetector.eyesInformation.RE.x - newDetector.eyesInformation.LE.x;
                double ang = atan(yvalue / xvalue) * (180 / CV_PI);

                if(pow(ang, 2) < 200)
                {
                    IplImage* im = newDetector.clipFace(queryImage);
                    send_msg(pamh, gettext("Verifying Face ..."));
                    if(im != 0)
                    {
                        int val = newVerifier->verifyFace(im);
                        if(val == 1)
                        {
                            *commAuth = STOPPED;
                            // cvSaveImage("/home/rohan/new1.jpg",newDetector.clipFace(queryImage));
                            send_msg(pamh, gettext("Verification successful."));

                            if(enableX == 1)
                            {
                                XDestroyWindow(displayScreen, window);
                                XCloseDisplay(displayScreen);
                            }

                            writeImageToMemory(zeroFrame, shared);
                            webcam.stopCamera();

                            t2 = (double)cvGetTickCount();

                            while(t3 < 1300) t3 = (double)cvGetTickCount() - t2;
                           
                            return PAM_SUCCESS;
                        }
                    }
                    cvReleaseImage(&im);
                }
                else send_msg(pamh, gettext("Align your face."));

                newWebcamImagePaint.paintCyclops(queryImage, 
                  newDetector.eyesInformation.LE, newDetector.eyesInformation.RE);
                newWebcamImagePaint.paintEllipse(queryImage, 
                  newDetector.eyesInformation.LE, newDetector.eyesInformation.RE);

                //  cvLine(queryImage, newDetector.eyesInformation.LE, 
                //    newDetector.eyesInformation.RE, cvScalar(0,255,0), 4);
            }
            else send_msg(pamh,gettext("Keep proper distance with the camera."));
            

            if(enableX == 1) processEvent(displayScreen, window, width, height, queryImage, s);

            writeImageToMemory(queryImage, shared);
            cvReleaseImage(&queryImage);
        }
        else send_msg(pamh, gettext("Unable query image from your webcam."));
    }
    
    writeImageToMemory(zeroFrame, shared);

    send_msg(pamh, gettext("Giving Up Face Authentication. Try Again."), 1);
    
    if(enableX == 1)
    {
        XDestroyWindow(displayScreen,window);
        XCloseDisplay(displayScreen);
    }
    
    *commAuth = STOPPED;
    webcam.stopCamera();
    
    return PAM_AUTHINFO_UNAVAIL;
}

//------------------------------------------------------------------------------
PAM_EXTERN
int pam_sm_setcred(pam_handle_t* pamh, int flags, int argc, const char** argv)
{
    return PAM_SUCCESS;
}

/* --- account management functions --- */
PAM_EXTERN
int pam_sm_acct_mgmt(pam_handle_t* pamh, int flags, int argc, const char** argv)
{
    return PAM_SUCCESS;
}

/* --- password management --- */
PAM_EXTERN
int pam_sm_chauthtok(pam_handle_t* pamh, int flags, int argc, const char** argv)
{
    return PAM_SUCCESS;
}

/* --- session management --- */
PAM_EXTERN
int pam_sm_open_session(pam_handle_t* pamh, int flags, int argc, const char** argv)
{
    return PAM_SUCCESS;
}

//------------------------------------------------------------------------------
PAM_EXTERN
int pam_sm_close_session(pam_handle_t* pamh, int flags, int argc, const char** argv)
{
    return PAM_SUCCESS;
}

