#include "cv.h"
#include "highgui.h"
 class opencvWebcam
    {
    public:
        opencvWebcam(void);
        ~opencvWebcam(void);
	IplImage *queryFrame();
int startCamera();
    private:
	CvCapture* capture;

    };
