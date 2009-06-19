#include "cv.h"
#include "highgui.h"
 class opencvWebcam
    {
    public:
        opencvWebcam(void);
	IplImage *queryFrame();

    private:
	CvCapture* capture;

    };
