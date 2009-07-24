/**
* Write Image To Memory
*@param Input Image
*@param Shared Memory Pointer
*/
void writeImageToMemory(IplImage*,char*);
/**
* Start IPC Connections
*/
void ipcStart();
/**
* Reset IPC Flags
*/
void resetFlags();

/**
* Pointer for Image Shared Memory
*/
char *shared;
key_t ipckey;
int shmid;

// COMMUNICATION SHARED MEMORY
/**
* Pointer for Communication Shared Memory
*/
int *commAuth;
key_t ipckeyCommAuth;
int shmidCommAuth;

// FLAGS
//char *QT_FACE_AUTH=BINDIR "/qt-faceauth &";
//char *QT_FACE_AUTH=BINDIR "/qt-faceauth &";
