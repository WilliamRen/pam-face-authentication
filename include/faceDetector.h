/** @file */

#include "cv.h"
#include "highgui.h"
/**
* Face Structure, Information that can describe the detected Face on the Image
*/
struct face
{
    CvPoint LT; /**< Co-ordinates of Left Top of the Face */
    CvPoint RB;/**< Co-ordinates of Right Top of the Face */
    int Width;/**< Width of the Face */
    int Height;/**< Height of the Face */
} ;
/**
* Face Detector Class. This Class Runs the OpenCV Haar Detect Functions for finding the Face.
*/
class faceDetector
{
public:
    /**
    * Face Information struct.
    *@see struct face
    */
    struct face faceInformation;
    /**
    * The Constructor
    *@see Initializes Structures for Haar Detection
    */
    faceDetector(void);
    /**
    *Runs the Face Detection Algorithm on the param image
    *@param input The Input image on which the algorithm should be run on.
    */
    void runFaceDetector(IplImage * input);
    /**
    *Returns the Face Image of the Detected Face
    *@param inputImage The Input image.
    *@result IplImage on success , 0 on failure
    */
    IplImage * clipDetectedFace(IplImage * inputImage);
    /**
    *Returns 1 or 0 , depending on Success or Failure of the detection algorithm
    *@result 1 on success , 0 on failure
    */
    int checkFaceDetected();
private:
    /**
    *OpenCV Cascade Structure
    */
    CvHaarClassifierCascade* cascade;
    /**
    *Storage - Work Area for the OpenCV detection algorithm
    */
    CvMemStorage* storage;

};
