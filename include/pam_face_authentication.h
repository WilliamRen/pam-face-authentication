/** @file */


/**
* Pipeline Function to stop Abusing the Terminal with Info/Error Messages
*@param msg New Message
*@return 1 on Success, 0 on Failure
*/
int msgPipeLiner(char* msg);


/**
* Send PAM Info Message Through Conversation
*@param pamh PAM Handle
*@param msg Message
*@return -1 on Failure
*/
static int send_info_msg(pam_handle_t *pamh, char *msg);

/**
* Send PAM Error Message Through Conversation
*@param pamh PAM Handle
*@param msg Message
*@return -1 on Failure
*/
static int send_err_msg(pam_handle_t *pamh, char *msg);

/**
* Write Image To Memory
*@param Input Image
*@param Shared Memory Pointer
*/
void writeImageToMemory(IplImage*Input,char*Shared);
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
/**
* Key for Image Shared Memory
*/
key_t ipckey;
/**
* Shared Memory ID for Image Shared Memory
*/
int shmid;


// COMMUNICATION SHARED MEMORY | NOT NEEDED ANYMORE
/**
* Pointer for Communication Shared Memory
*/
int *commAuth;
/**
* Key for Communication Shared Memory
*/
key_t ipckeyCommAuth;
/**
* Shared Memory ID for Communication Shared Memory
*/
int shmidCommAuth;

// FLAGS
//char *QT_FACE_AUTH=BINDIR "/qt-faceauth &";
//char *QT_FACE_AUTH=BINDIR "/qt-faceauth &";
