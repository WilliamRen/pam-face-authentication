/** @file */

#include "cv.h"

/**
* Eye Structure, Information that can describe the detected Eye on the Face Image
*/
struct eyes
{
    CvPoint LE; /**< Co-ordinates of the Left Eye */
    CvPoint RE; /**< Co-ordinates of the Right Eye */
    int Length; /**< Length Eye */
};
/**
* Eye Detector Class. This Class Runs the OpenCV Haar Detect Functions for finding Eyes.
*/
class eyesDetector
{
public:
    /**
    * Eye Information that can describe the detected Eye on the Face Image
    @see struct eyes
    */
    struct eyes eyesInformation;

    /**
    *The Constuctor
    *Initializes internal variables
    */
    eyesDetector(void);
    /**
    *function to run the Detection Algorithm on param image
    *@param input The IplImage on which the Algorithm should be run on
    *@return 1 if success , 0 for failure
    */
    void runEyesDetector(IplImage * input,IplImage * fullImage,CvPoint LE);
    /**
    *Returns 1 or 0 , depending on Success or Failure of the detection algorithm
    *@result 1 on success , 0 on failure
    */
    int checkEyeDetected();




private:
    /**
    *Eye Cascade Structure 1
    */
    CvHaarClassifierCascade* nested_cascade;
    /**
    *Eye Cascade Structure 2
    */
    CvHaarClassifierCascade* nested_cascade_2;
    /**
    *Work Area for Haar Detection
    */
    CvMemStorage* storage;

    /**
    *Internal Variable to Track if Both eyes were detected
    *@see checkEyeDetected
    */
    int bothEyesDetected;

};
