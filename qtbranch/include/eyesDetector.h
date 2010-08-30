#ifndef _INCL_EYES_DETECTOR
#define _INCL_EYES_DETECTOR

/**
* Eye structure. Information that describe the detected eyes on the face image
*/
struct eyes
{
    CvPoint LE; /** Coordinates of the Left Eye */
    CvPoint RE; /** Coordinates of the Right Eye */
    int Length; /** Length Eye */
};

/**
* Eye detector class. This class runs the OpenCV Haar detection functions for finding eyes.
*/
class eyesDetector
{
  private:
      /**
      *Eye Cascade Structures
      */
      CvHaarClassifierCascade* nested_cascade_;
      CvHaarClassifierCascade* nested_cascade_2_;
      
      /**
      * Work area for Haar detection
      */
      CvMemStorage* storage_;

      /**
      * Internal Variable to track if both eyes were detected
      * @see checkEyeDetected
      */
      bool bothEyesDetected_;
      
      /**
      * Copy constructor
      * @param eyesDetector, original value to copy
      */
      eyesDetector(eyesDetector&);

      /**
      * Assignment operator
      * @param eyesDetector, original value to assign
      */
      eyesDetector& operator =(const eyesDetector&);      

  public:
    /**
    * Eye information that describes the detected eye on the face image
    * @see struct eyes
    */
    struct eyes eyesInformation;
    
    /**
    * The constuctor
    * Initializes internal variables
    */
    eyesDetector();
    
    /**
    * The destructor
    */
    virtual ~eyesDetector();
    
    /**
    * Function to run the detection algorithm on param image
    * @param input The IplImage on which the algorithm should be run on
    */
    void runEyesDetector(IplImage* input, IplImage* fullImage, CvPoint LE);
    
    /**
    * Depending on success or failure of the detection algorithm, a state is returned
    * @result true on success, 0 on failure
    */
    bool checkEyeDetected();
};

#endif // _INCL_EYES_DETECTOR
