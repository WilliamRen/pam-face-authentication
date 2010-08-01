#ifndef _INCL_GUARD_DETECTOR
#define _INCL_GUARD_DETECTOR

#include <string>
#include "cv.h"
#include "tracker.h"
#include "pam_face_defines.h"
#include "faceDetector.h"
#include "eyesDetector.h"

/**
* Detector class. This class subclasses the face and eyes detector classes.
* @see faceDetector
* @see eyesDetector
*/
class detector: public faceDetector, public eyesDetector
{
  private:
    /**
    * This class subclasses the face and eyes detector classes. This is used for 
    * capturing face images in training.
    * @see clippedFace
    */
    int boolClipFace_;

    /**
    * Total number of faces to be clipped.
    */
    int totalFaceClipNum_;
    
    /**
    * Number of face images clipped till now.
    */
    int clipFaceCounter_;

    /**
    * If all the faces are clipped: 1 otherwise 0
    * @see totalFaceClipNum
    */
    int finishedClipFaceFlag_;

    /**
    * Eye tracker object for the left and right eye
    */
    tracker leftEye_;
    tracker rightEye_;

    /**
    * Eye coordinates
    */
    CvPoint leftEyeP_, rightEyeP_;
    
    /**
    * Eye coordinates relative to face
    */
    CvPoint leftEyePointRelative_, rightEyePointRelative_;

    /**
    * Face angle
    */
    double inAngle_;

    /**
    * Length of eye
    */
    int lengthEye_, widthEyeWindow_, heightEyeWindow_;
   
    /**
    * Length of eye in last frame
    */
    int prevlengthEye_;

    /**
    * Temp variable used to print %d/%d message while capturing images
    */
    std::string messageCaptureMessage_;

    /**
    * Copy constructor
    * @param detector, original value to copy
    */
    detector(detector&);

    /**
    * Assignment operator
    * @param detector, original value to assign
    */
    detector& operator =(const detector&);

  public:
    /**
    * The constuctor
    * Initializes internal variables
    */
    detector();

    /**
    * The destructor
    */
    virtual ~detector();

    /**
    * Array of clipped face images
    */
    IplImage** clippedFace;

    /**
    * Message index corresponding to different states
    */
    int messageIndex;

    /**
    * Getter for ClipFaceCounter
    * @return clipFaceCounter
    */
    int getClipFaceCounter();

    /**
    * Function to return the pointers to the clipped faces
    * @return returns a set of IplImage of clipped faces
    */
    IplImage** returnClipedFace();

    /**
    * Starts the clipping of detected faces
    * @param num total number of face images to be clipped
    */
    void startClipFace(int num);

    /**
    * Stops the clipping
    */
    void stopClipFace();

    /**
    * Function to check if prespecified faces have been clipped
    * @return 1 if success, 0 on failure
    * On success, code should do the work on the images, 
    * otherwise it might get lost forever
    */
    int finishedClipFace();

    /**
    * Function to run the detection / tracking algorithm on param image
    * @param input, the IplImage on which the algorithm should be run on
    * @return 1 if success, 0 on failure
    */
    int runDetector(IplImage* input);

    /**
    * Function to return the current state of detector
    * @return messageIndex
    */
    int queryMessage();

    /**
    * Returns the face image of the detected face
    * @param inputImage, the input image.
    * @result IplImage on success, 0 on failure
    */
    IplImage* clipFace(IplImage* inputImage);
    
    /**
    * Returns the face image of the detected face
    * @result 1 on success, 0 on failure
    */
    int detectorSuccessful();
};

#endif // _INCL_GUARD_DETECTOR

