#ifndef _INCL_EYES_DETECTOR
#define _INCL_EYES_DETECTOR

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
      * Work area for Haar detection
      */
      CvMemStorage* storage;

      /**
      * Internal Variable to track if both eyes were detected
      * @see checkEyeDetected
      */
      bool bothEyesDetected;
      
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
