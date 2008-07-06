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



// LIBRARY FOR HACK TO SEE IF X SERVER IS ONLINE
#include <X11/Xlib.h>



// PATH

char *path="/lib/pamface/";


// WEBCAM IMAGE SHARED MEMORY
char *shared;
key_t ipckey;
int shmid;


// COMMUNICATION SHARED MEMORY
int *commAuth;
key_t ipckeyCommAuth;
int shmidCommAuth;

// AUTHENTICATION && CANCEL BUTTON  FLAGS
int AuthenticateButtonClicked=0;
int CancelButtonClicked=0;



//  FACE DETECTION
static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;


////////////////////////////////////////////////////////////////////////
char startTracker(char*);
void writeImageToMemory(IplImage*,char*);
void ipcStart();
void setFlags();
////////////////////////////////////////////////////////////////////////


void setFlags()
{
 if((*commAuth)==14)
 {
CancelButtonClicked=1;
 }
if((*commAuth)==7)
{
AuthenticateButtonClicked=1;
}
}


void ipcStart()
{

/*   IPC   */
    ipckey =  567814;
    shmid = shmget(ipckey, 230400, IPC_CREAT | 0666);
    shared = shmat(shmid, NULL, 0);
    ipckeyCommAuth = 567813;
    shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
    commAuth = shmat(shmidCommAuth, NULL, 0);
    *commAuth=0;
/*   IPC END  */
}

void writeImageToMemory(IplImage* img,char *shared)
{

int m,n;

     for(n=0;n<240;n++)
 {
      for(m= 0;m<320;m++)
 {
      CvScalar s;
s=cvGet2D(img,n,m);
*(shared + m*3 + 0+ n*320*3)=(uchar)s.val[0];
*(shared + m*3 + 1+ n*320*3)=(uchar)s.val[1];
*(shared + m*3 + 2+ n*320*3)=(uchar)s.val[2];


 }
 }
}





char startTracker(char* username)
{

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


// DLOPEN libfacedetect which contains the face detection related methods
void *handleFaceDetect;
void (*faceDetect)(IplImage*,CvPoint*,CvPoint*,CvMemStorage*,CvHaarClassifierCascade*);
void (*saveFace)(IplImage*,CvPoint*,CvPoint*,char*,char*);
handleFaceDetect = dlopen("/lib/pamface/libfacedetect.so", RTLD_NOW);
faceDetect = dlsym(handleFaceDetect, "faceDetect");
saveFace = dlsym(handleFaceDetect, "saveFace");
// DLOPEN ENDS HERE

// DLOPEN faceauthenticate
	void *handleAuthenticate;
    char (*recognize)(char*);
     handleAuthenticate = dlopen ("/lib/pamface/faceauthenticate.so", RTLD_NOW);
	handleAuthenticate = dlopen ("/lib/pamface/faceauthenticate.so", RTLD_NOW);
	recognize = dlsym(handleAuthenticate, "recognize");

    //dlclose(handleAuthenticate);
// DLOPEN ENDS HERE


//  DETECTED FACE CORDINATES
   CvPoint p1;
   CvPoint p2;
   cascade = (CvHaarClassifierCascade*)cvLoad( "/lib/pamface/haarcascade.xml", 0, 0, 0 );
    storage = cvCreateMemStorage(0);
////

 //cvNamedWindow( "Face Tracker", 1 );  // TEST

   CvCapture* capture = 0;
    IplImage *frame, *frame_copy = 0;
    capture = cvCaptureFromCAM(0);




    if( capture )
    {
        for(;;)
        {
            if( !cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture );
            if( !frame )
                break;
            if( !frame_copy )
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height),IPL_DEPTH_8U, frame->nChannels );
            if( frame->origin == IPL_ORIGIN_TL )
                cvCopy( frame, frame_copy, 0 );
            else
                cvFlip( frame, frame_copy, 0 );
         cvClearMemStorage( storage );
         faceDetect(frame_copy,&p1,&p2,storage,cascade);
        setFlags();

      //   cvShowImage( "Face Tracker", frame_copy);
        // if( cvWaitKey( 4 ) >= 0 )
         // break;
//       faceDetect(frame_copy,username);
          if(AuthenticateButtonClicked==1)
            {
                saveFace(frame_copy,&p1,&p2,path,username);
                if(recognize(username)=='y')
                {

                    cvReleaseImage( &frame_copy );
                    cvReleaseCapture( &capture );
                    return 'y';
                }

                AuthenticateButtonClicked=0;
                *commAuth=0;

            }
               if(CancelButtonClicked==1)
            {
                CancelButtonClicked=0;
                *commAuth=0;

                cvReleaseImage( &frame_copy );
                cvReleaseCapture( &capture );
                return 'n';

            }
        cvRectangle(frame_copy,p1,p2,colors[3],3,4,0);
        writeImageToMemory(frame_copy,shared);
        }

        cvReleaseImage( &frame_copy );
        cvReleaseCapture( &capture );
    }

return 'n';
}
void removeFile(char* userTemp)
{

     char* userFile;
     userFile=(char *)calloc(strlen(userTemp) + 18,sizeof(char));
     strcat(userFile,"/lib/pamface/");;
     strcat(userFile,userTemp);
     strcat(userFile,".pgm");
     remove(userFile);

}
PAM_EXTERN
int pam_sm_authenticate(pam_handle_t *pamh,int flags,int argc
			,const char **argv)
{
    int retval;
     const char *user=NULL;



    char *userTemp;
    char *userFile;
		const char *error;

    /*
     * authentication requires we know who the user wants to be
     */

    retval = pam_get_user(pamh, &user, NULL);
    if (retval != PAM_SUCCESS) {
	D(("get user returned error: %s", pam_strerror(pamh,retval)));
	return retval;
    }
    if (user == NULL || *user == '\0') {
	D(("username not known"));
	pam_set_item(pamh, PAM_USER, (const void *) DEFAULT_USER);
    }

ipcStart(); // START IPC [shared memory]





 Display *d;
/*HACK to see if XServer is Online :  open connection with the server */
   d=XOpenDisplay(NULL); // NULL means it will use the $DISPLAY environment variable
   if(d==NULL)
   {

system("/lib/pamface/cmd-facetracker");
// RUN THE COMMAND LINE VERSION .. :(


   }
   else
   {

system("/lib/pamface/gtk-facetracker &");
// RUN THE GTK VERSION.. YIPEEEE!
   }

//// NEED TO FIX
userTemp=(char *)calloc(strlen(user)+1,sizeof(char));
strcpy(userTemp,user);
removeFile(userTemp);
////



if(startTracker(userTemp)=='y')
  { // printf(" y %s",user);
  return PAM_SUCCESS;

  }
  else
  {

     return PAM_AUTH_ERR;
  }
 /*
    int shmidCommAuth;
    key_t ipckeyCommAuth;
    int *commAuth;
    ipckeyCommAuth = 567813;
    shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
    commAuth = shmat(shmidCommAuth, NULL, 0);
   if(*commAuth==14)
   loop=-1;

*/

                            /* clean up */





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

/* end of module definition */

//#ifdef PAM_STATIC

/* static module data */
/* Code was a left over from pam_permit
struct pam_module _pam_permit_modstruct = {
    "pam_permit",
    pam_sm_authenticate,
    pam_sm_setcred,
    pam_sm_acct_mgmt,
    pam_sm_open_session,
    pam_sm_close_session,
    pam_sm_chauthtok
};

#endif

*/
