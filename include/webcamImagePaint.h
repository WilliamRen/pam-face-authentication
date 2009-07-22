#include "cv.h"
#include "highgui.h"
#ifndef _INCL_GUARD_PAINT
#define _INCL_GUARD_PAINT

 class webcamImagePaint
    {
    public:
        webcamImagePaint(void);
        void paintCyclops (IplImage *,CvPoint leftEye,CvPoint rightEye);
		void paintEllipse(IplImage *image,CvPoint leftEye,CvPoint rightEye);
    };
#endif