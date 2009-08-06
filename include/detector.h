/** @file */

#include "cv.h"
#ifndef _INCL_GUARD_DET
#define _INCL_GUARD_DET
#include "tracker.h"
#include "pam_face_defines.h"

#include "faceDetector.h"
#include "eyesDetector.h"

/**
* Detector Class. This Class subclasses the face and eyes detector Classes.
@see faceDetector
@see eyesDetector
*/
class detector: public faceDetector, public eyesDetector
{
private:
    /**
    * This Class subclasses the face and eyes detector Classes. This is used in Capturing Face Images in Training.
    @see clippedFace
    */
    int boolClipFace;
    /**
    * Total Number of Faces to be Clipped.
    */
    int totalFaceClipNum;
    /**
    * Number of Face Images Clipped till now.
    */
    int clipFaceCounter;
    /**
    * If all the faces are Clipped 1 otherwise 0
    *@see totalFaceClipNum
    */
    int finishedClipFaceFlag;

public:
    /**
    * Array of Clipped Face Images
    */
    IplImage**clippedFace;
    /**
    * Message Index corresponding to Different States
    */
    int messageIndex;
    /**
    * Function to return the Pointers to the Clipped Faces
    *@return returns a set of IplImage of Clipped Faces
    */
    IplImage **returnClipedFace();
    /**
    * Starts the Clipping Detected Faces
    *@param num total number of face images to be clipped
    */
    void startClipFace(int num);
    /**
    * Stops the Clipping
    */
    void stopClipFace();

    /**
    *function to Check if Prespecified of Faces has been Clipped
    *@return 1 if success , 0 for failure, On Success code should do the work on the Images , otherwise it might get Lost forever
    */
    int finishedClipFace();

    /**
    *The Constuctor
    *Initializes internal variables
    */
    detector(void);

    /**
    *function to run the Detection / Tracking Algorithm on param image
    *@param input The IplImage on which the Algorithm should be run on
    *@return 1 if success , 0 for failure
    */
    int runDetector(IplImage * input);

    /**
    *function to return the Current State of Detector
    *@return the String which describes the State in Human Readable Form (english ( need to fix this, translation))
    */
    char * queryMessage();

    /**
    *Returns the Face Image of the Detected Face
    *@param inputImage The Input image.
    *@result IplImage on success , 0 on failure
    */
    IplImage * clipFace(IplImage * inputImage);
    /**
    *Returns the Face Image of the Detected Face
    *@result 1 on success , 0 on failure
    */
    int detectorSuccessful();
private:

    /**
    *Eye tracker object for Left eye
    */
    tracker leftEye;

    /**
    *Eye tracker object for Right eye
    */
    tracker rightEye;
    /**
    *Eye Cordinates
    */
    CvPoint leftEyeP,rightEyeP;
    /**
    *Relative to Face ,Eye Cordinates
    */
    CvPoint leftEyePointRelative,rightEyePointRelative;
    /**
    *Angle of Face
    */
    double inAngle;
    /**
    *Length Of Eye
    */
    int lengthEye,widthEyeWindow,heightEyeWindow;
     /**
    *Length Of Eye in Last Frame
    */
    int prevlengthEye;

    /**
    *Temp Variable used to print %d/%d message while capturing images
    */
    char messageCaptureMessage[300];
};
#endif
