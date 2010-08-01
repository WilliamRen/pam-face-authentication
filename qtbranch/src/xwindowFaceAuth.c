/** @file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define IPC_KEY_IMAGE 567814  // Shared Memory Key Value for Image
#define IPC_KEY_STATUS 567813 // Shared Memory Key Value for Communication
#define IMAGE_SIZE 230400 // Shared Memory Size for Image , 320X240
#define IMAGE_WIDTH 320  // Image Width of Webcam
#define IMAGE_HEIGHT 240 // Image Height of Webcam
#define STARTED 21

//------------------------------------------------------------------------------
/// Prototypes
void ipcStart();  // Start IPC Connections
void resetFlags();  // Reset IPC Flags

//------------------------------------------------------------------------------
/// Global variables
char *shared;  // Pointer for Image Shared Memory
key_t ipckey;  // Key for Image Shared Memory
int shmid;     // Shared Memory ID for Image Shared Memory
int *commAuth; // Pointer for Communication Shared Memory
key_t ipckeyCommAuth;  // Key for Communication Shared Memory
int shmidCommAuth;  // Shared Memory ID for Communication Shared Memory

//------------------------------------------------------------------------------
void resetFlags()
{
  *commAuth = 0;
}

//------------------------------------------------------------------------------
void ipcStart()
{
  // IPC - Start
  ipckey =  IPC_KEY_IMAGE;
  shmid = shmget(ipckey, IMAGE_SIZE, IPC_CREAT | 0666);
  shared = (char *)shmat(shmid, NULL, 0);

  ipckeyCommAuth = IPC_KEY_STATUS;
  shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
  commAuth = (int *)shmat(shmidCommAuth, NULL, 0);
  // IPC - End
}

//------------------------------------------------------------------------------
XImage *CreateTrueColorImage(Display *display, Visual *visual, 
  char **image32, int width, int height)
{
  int i, j;
  char *p = *image32;
  
  for(j = 0; j < height; j++)
  {
    for(i = 0; i < width; i++)
    {
      *p++= *(shared + i*3 + 0 + j*IMAGE_WIDTH*3) % 256; // blue
      *p++= *(shared + i*3 + 1 + j*IMAGE_WIDTH*3) % 256; // green
      *p++= *(shared + i*3 + 2 + j*IMAGE_WIDTH*3) % 256; // green

      p++;
    }
  }
  
  return XCreateImage(display, visual, 24, ZPixmap, 0,
    (unsigned char*)image32, width, height, 32, 0);
}

//------------------------------------------------------------------------------
void processEvent(Display *display, Window window, 
  int width, int height, int screen)
{
  XImage* ximage;
  char* image32 = (char *)malloc(width * height * 4);
    
  Visual *visual = DefaultVisual(display, screen);
  ximage = CreateTrueColorImage(display, visual, &image32, width, height);
  XPutImage(display, window, DefaultGC(display, 0), ximage, 0, 0, 0, 0, width, height);
  XDestroyImage(ximage);

  if(image32 != 0) free(image32);
}

//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
  XImage *ximage;
  Window window;
  Display *display;
  int width = IMAGE_WIDTH, height = IMAGE_HEIGHT, screen, k;
    
  display = XOpenDisplay(NULL);
  screen = DefaultScreen(display);

  ipcStart();

  if(argc >= 2)
  {
    k = atoi(argv[1]);
    window = XCreateSimpleWindow(display, k, 0, 0, width, height, 1, 0, 0);
  }
  else
  {
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 
      0, 0, width, height, 1, 0, 0);
  }

  XMapWindow(display, window);
  while(*commAuth == STARTED) 
    processEvent(display, window, width, height, screen);

  XUnmapWindow(display, window);
  XDestroyWindow(display,window);
  XCloseDisplay(display);

  return 0;
}

