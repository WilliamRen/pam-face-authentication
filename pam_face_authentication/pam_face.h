////////////////////////////////////////////////////////////////////////
void allocateMemory();
void intialize();
char startTracker(int *,char*);
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



