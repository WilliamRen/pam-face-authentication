/** @file */

#include "cv.h"
#include "highgui.h"
#ifndef _INCL_GUARD_PAINT
#define _INCL_GUARD_PAINT
/**
* Webcam Image Paint Class. This Class contains functions to paint over the image , to show different effects.
*/
class webcamImagePaint
{
public:
    /**
    *The Constructor
    *Currently does nothing
    */
    webcamImagePaint(void);
    /**
    *Paints the Cyclops Effect
    *@param image Image to be painted upon
    *@param leftEye left eye cordinates
    *@param rightEye right eye cordinates
    */
    void paintCyclops (IplImage *,CvPoint leftEye,CvPoint rightEye);
    /**
    *Paints the Spotlight Ellipse Effect
    *@param image Image to be painted upon
    *@param leftEye left eye cordinates
    *@param rightEye right eye cordinates
    */
    void paintEllipse(IplImage *image,CvPoint leftEye,CvPoint rightEye);
};
#endif
