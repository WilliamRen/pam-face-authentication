#include "cv.h"
#include "highgui.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h> /* getpwdid */
#include <time.h>
#ifndef _INCL_GUARD_VER
#define _INCL_GUARD_VER


typedef struct {
IplImage** faces;
int count;
}structFaceImages;


typedef struct {
char **setName;
structFaceImages *faceImages;
char **setFilePathThumbnails;
int count;
}setFace;



/*
typedef struct {
IplImage **faceImages;
int count;
}allFaces;
*/
class verifier
{
public:
verifier();
verifier(uid_t   userID);
void createMaceFilter();
//Done
void addFaceSet(IplImage **set,int size);
//allFaces* getFaceImagesFromAllSet();
//Done
void removeFaceSet(char* setName);
//Done
setFace* getFaceSet();


//TODO
int verifyFace(IplImage *face);
char facesDirectory[200];
char modelDirectory[200];
char configDirectory[200];


private:

char * createSetDir();
struct passwd *userStruct;
};
#endif
