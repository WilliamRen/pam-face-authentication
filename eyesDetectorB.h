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
    double *integralImageYLeft;
    double *integralImageYRight;
    double *integralImageXLeft;
    double *integralImageXRight;
    int integralImageSizeY;
    int integralImageSizeX;

    CvPoint integralEyeLeft;
    CvPoint integralEyeRight;

    struct eyes eyesInformation;
    eyesDetector(void);
    double runGridSearch(IplImage *gray,int size,int flag,double *d,double* e ,double * integralTemp);
    double * calculateIntegral(IplImage * input,int flag);
    void runEyesDetector(IplImage * input,IplImage * fullImage,CvPoint LE);
    int checkEyeDetected();
    double gridSearch(double  initialPointX,double initialPointY,double * finalPointX,double * finalPointY,double *currentIntegralImage,double* integralModel,int size,double scale,double translate);



private:
    CvHaarClassifierCascade* nested_cascade;
    CvHaarClassifierCascade* nested_cascade_2;
    CvMemStorage* storage;

};
