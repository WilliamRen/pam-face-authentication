#include "cv.h"
#include "highgui.h"
#ifndef _INCL_GUARD_WEBCAM
#define _INCL_GUARD_WEBCAM

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
#endif
