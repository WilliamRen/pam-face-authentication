#ifndef _INCL_PFA_PLUGIN
#define _INCL_PFA_PLUGIN

int file_exists(const char* filename);
char* prevmsg = 0;
char* shared; // Pointer for image shared memory
bool boolMessages = true;
key_t ipckey; // Key for Image Shared Memory
key_t ipckeyCommAuth; // Key for Communication Shared Memory
int shmidCommAuth; // Shared Memory ID for Communication Shared Memory
int shmid; // Shared Memory ID for Image Shared Memory

// COMMUNICATION SHARED MEMORY | NOT NEEDED ANYMORE
int* commAuth; // Pointer for Communication Shared Memory

// FLAGS
//char *QT_FACE_AUTH=BINDIR "/qt-faceauth &";
//char *QT_FACE_AUTH=BINDIR "/qt-faceauth &";


/**
* Pipeline function to stop abusing the terminal with info/error messages
* @param msg New message
* @return true on success, false on failure
*/
bool msgPipeLiner(char* msg);

/**
* Send PAM Info message through conversation
* @param pamh PAM Handle
* @param msg Message
* @param type, defines whether the message is an error (1) or not (0)
* @return -1 on Failure
*/
static int send_msg(pam_handle_t* pamh, char* msg, int type = 0);


/**
* Write image to memory
* @param Input image
* @param Shared Memory Pointer
*/
void writeImageToMemory(IplImage* Input, char* Shared);

/**
* Start IPC Connections
*/
void ipcStart();

/**
* Reset IPC Flags
*/
void resetFlags();

#endif // _INCL_PFA_PLUGIN
