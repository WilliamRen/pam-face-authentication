#include "cv.h"
#include "highgui.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h> /* getpwdid */
#include <time.h>

typedef struct{
int filterMaceFacePSLR;
int filterMaceEyePSLR;
int filterMaceInsideFacePSLR;
}config;

typedef struct {
char **setName;
char **setFilePathThumbnails;
int count;
}setFace;
typedef struct {
IplImage **faceImages;
int count;
}allFaces;

class verifier
{
public:
verifier();
//^^CONFIG LATER =)
void setConfig(config *configuration);
config * getConfig();

//^^CONFIG LATER =)

void createMaceFilter();
//Done
void addFaceSet(IplImage **set,int size);
allFaces* getFaceImagesFromAllSet();
//Done
void removeFaceSet(char* setName);
//Done
setFace* getFaceSet();


//TODO
int verifyFace(IplImage *face);



private:
char facesDirectory[200];
char modelDirectory[200];
char configDirectory[200];
char * createSetDir();
struct passwd *userStruct;
};
