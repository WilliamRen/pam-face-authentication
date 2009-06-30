////////////////////////////////////////////////////////////////////////
void writeImageToMemory(IplImage*,char*);
void ipcStart();
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
//char *QT_FACE_AUTH=BINDIR "/qt-faceauth &";
char *QT_FACE_AUTH=BINDIR "/gtk-facetracker &";
