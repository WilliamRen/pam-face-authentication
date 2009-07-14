#include "cv.h"
#ifndef _INCL_GUARD_DET
#define _INCL_GUARD_DET

#include "faceDetector.h"
#include "eyesDetector.h"

struct paintDescription
{
    CvPoint LT;
    CvPoint LB;
    CvPoint RT;
    CvPoint RB;
    CvPoint LE;
    CvPoint RE;
    int Length;
    int Width;
    int Height;

} ;

 class detector: public faceDetector, public eyesDetector
    {
    private:
    int boolClipFace;
    int totalFaceClipNum;
    int clipFaceCounter;
    int finishedClipFaceFlag;

    public:
    IplImage**clippedFace;
    int messageIndex;

    IplImage * *returnClipedFace();
    void startClipFace(int num);
    void stopClipFace();
    int finishedClipFace();
    struct paintDescription paintInformation;
    detector(void);
	int runDetector(IplImage * input);
    char * queryMessage();
    IplImage * clipFace(IplImage * inputImage);
    int detectorSuccessful();
 private:
 char messageCaptureMessage[300];
    };
#endif
