/** @file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/**
* Shared Memory Key Value for Image
*/
#define IPC_KEY_IMAGE 567814
/**
* Shared Memory Key Value for Communication
*/
#define IPC_KEY_STATUS 567813
/**
* Shared Memory Size for Image , 320X240
*/
#define IMAGE_SIZE 230400

/**
* Image Width of Webcam
*/
#define IMAGE_WIDTH 320
/**
* Image Height of Webcam
*/
#define IMAGE_HEIGHT 240
#define STARTED 21

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
    /*   IPC END  */
}
XImage *CreateTrueColorImage(Display *display, Visual *visual, unsigned char *image, int width, int height)
{
    int i, j;
    char *image32=(unsigned char *)malloc(width*height*4);
    char *p=image32;
    for (j=0; j<height; j++)
    {
        for (i=0; i<width; i++)
        {

            *p++= *(shared + i*3+ 0+ j*IMAGE_WIDTH*3)%256; // blue
            *p++= *(shared + i*3 + 1+ j*IMAGE_WIDTH*3)%256; // green
            *p++= *(shared + i*3 + 2+ j*IMAGE_WIDTH*3)%256; // green

            p++;
        }
    }
    return XCreateImage(display, visual, 24, ZPixmap, 0,(unsigned char *) image32, width, height, 32, 0);
}

void processEvent(Display *display, Window window, int width, int height,int s )
{
    XImage *ximage;
    Visual *visual=DefaultVisual(display, s);
    ximage=CreateTrueColorImage(display, visual, 0, width, height);
    XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 0, 0, width, height);
    XDestroyImage(ximage);
}


int main(int argc, char **argv)
{
    XImage *ximage;
    int width=IMAGE_WIDTH, height=IMAGE_HEIGHT;
    Display *display;
    int  s;
    display=XOpenDisplay(NULL);
    s = DefaultScreen(display);

    ipcStart();
    Window window;
    if (argc>=2)
    {
        int k=atoi(argv[1]);
        window=XCreateSimpleWindow(display,k, 0, 0, width, height, 1, 0, 0);
    }
    else
    {
        window=XCreateSimpleWindow(display, RootWindow(display, s), 0, 0, width, height, 1, 0, 0);
    }
    XMapWindow(display, window);
    while (*commAuth==STARTED)
        processEvent(display, window, width, height,s);

    XUnmapWindow(display, window);
    XDestroyWindow(display,window);
    XCloseDisplay(display);
}
