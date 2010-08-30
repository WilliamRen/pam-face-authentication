#ifndef _INCL_FACE_DETECTOR
#define _INCL_FACE_DETECTOR

#include "cv.h"

/**
* Face structure, information that can describe the detected face on the image
*/
struct face
{
  CvPoint LT;  /** Co-ordinates of Left Top of the Face */
  CvPoint RB;  /** Co-ordinates of Right Top of the Face */
  int Width;   /** Width of the Face */
  int Height;  /** Height of the Face */
};

/**
* Face Detector class. This class runs the OpenCV Haar detect functions 
* for finding the face.
*/
class faceDetector
{
  private:
    /**
    * OpenCV cascade structure
    */
    CvHaarClassifierCascade* cascade_;
    
    /**
    * Storage - Work area for the OpenCV detection algorithm
    */
    CvMemStorage* storage_;
    
    /**
    * Copy constructor
    * @param faceDetector, original value to copy
    */
    faceDetector(faceDetector&);

    /**
    * Assignment operator
    * @param faceDetector, original value to assign
    */
    faceDetector& operator =(const faceDetector&);

  public:
    /**
    * Face information struct.
    * @see struct face
    */
    struct face faceInformation;

    /**
    * The Constructor
    * @see Initializes structures for Haar detection
    */
    faceDetector();

    /**
    * The destructor
    */
    virtual ~faceDetector();

    /**
    * Runs the face detection algorithm on the param image
    * @param input, The input image on which the algorithm should be run on.
    */
    void runFaceDetector(IplImage* input);
    
    /**
    * Returns the Face image of the detected face
    * @param inputImage, The Input image.
    * @result IplImage on success, 0 on failure
    */
    IplImage* clipDetectedFace(IplImage* inputImage);
    
    /**
    * Returns 1 or 0 , depending on Success or Failure of the detection algorithm
    * @result true on success, false on failure
    */
    bool checkFaceDetected();
};

#endif // _INCL_FACE_DETECTOR
