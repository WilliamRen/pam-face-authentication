////////////////////////////////////////////////////////////////////////
void writeImageToMemory(IplImage*,char*);
void ipcStart();
void setFlags();
void resetFlags();
////////////////////////////////////////////////////////////////////////


char *shared;
key_t ipckey;
int shmid;

// COMMUNICATION SHARED MEMORY
int *commAuth;
key_t ipckeyCommAuth;
int shmidCommAuth;

// FLAGS
int AuthenticateButtonClicked=0;
int CancelButtonClicked=0;
int displayErrorFlag=0;
void *funcRecognition(void );

char fullPath[300];
CvFont myFont;
CvPoint pts[4];
CvPoint pLeftEye,pRightEye;
int authenticateThreadReturn=0;
int threadNumber=0;
char *username;
int UserIdRecognition;
int authNo=0;
int authYes=0;

GMainLoop *loop;


char *GTK_FACE_AUTHENTICATE=BINDIR "/gtk-facetracker &";

