#ifndef _INCL_QTUTILS
#define _INCL_QTUTILS

#include <QtGui>
#include "cv.h"
#include "highgui.h"

/**
* Convert IplImage to QImage
* @param input Input Image
* @result Output QImage
*/
QImage* QImageIplImageCvt(IplImage* input);

#endif // _INCL_QTUTILS
