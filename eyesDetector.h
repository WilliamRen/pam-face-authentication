#include "cv.h"


struct eyes
{
    CvPoint LE;
    CvPoint RE;
    int Length;
};

class eyesDetector
{
public:
    struct eyes eyesInformation;
    eyesDetector(void);
    void runEyesDetector(IplImage * input,IplImage * fullImage,CvPoint LE);
    int checkEyeDetected();



private:
    CvHaarClassifierCascade* nested_cascade;
    CvHaarClassifierCascade* nested_cascade_2;
    CvMemStorage* storage;

};
