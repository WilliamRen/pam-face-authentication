#include "cv.h"
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
    int messageIndex;
    int boolClipFace;
    int totalFaceClipNum;
    int clipFaceCounter;
    int finishedClipFaceFlag;

    public:
    IplImage**clippedFace;

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
