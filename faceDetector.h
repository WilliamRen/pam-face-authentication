#include "cv.h"
#include "highgui.h"
struct face
{
    CvPoint LT;
    CvPoint RB;
    int Width;
    int Height;
} ;

class faceDetector
{
public:
    struct face faceInformation;
    faceDetector(void);
    void runFaceDetector(IplImage * input);
    IplImage * clipDetectedFace(IplImage * inputImage);
    int checkFaceDetected();

    int frameSkipNumber;
private:
    CvHaarClassifierCascade* cascade;
    CvMemStorage* storage;

};
