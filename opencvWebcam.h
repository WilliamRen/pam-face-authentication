#include "cv.h"
#include "highgui.h"
 class opencvWebcam
    {
    public:
        opencvWebcam(void);
	IplImage *queryFrame();
int startCamera();
void stopCamera();
    private:
	CvCapture* capture;

    };
