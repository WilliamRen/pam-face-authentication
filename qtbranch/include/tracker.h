#ifndef _INCL_GUARD_TRACK
#define _INCL_GUARD_TRACK

#include "cv.h"

#define NUMBER_OF_GRID_POINTS 25
#define GRID_SIDE_SIZE 5

/**
* Tracker class. This class implements an experimental object tracker.
* It uses variance and integral projection in X and Y directions as features
* and uses grid search algorithm to fit the model feature to the input feature.
*/
class tracker
{
  private:
    /**
    * Scale in X  - Scale*(Co-ordinate) + Translate
    */
    double stateVariableScaleX;

    /**
    * Translate in X - Scale*(Co-ordinate) + Translate
    */
    double stateVariableTranslateX;

    /**
    * Scale in Y  - Scale*(Co-ordinate) + Translate
    */
    double stateVariableScaleY;

    /**
    * Translate in Y  - Scale*(Co-ordinate) + Translate
    */
    double stateVariableTranslateY;

    /**
    * Image width and height of last track image call
    */
    double lastImageWidth;
    double lastImageHeight;

    /**
    * Calculate the Features
    * @param input, input image for which the feature should be calculated
    * @param flag, direction, X 0 or Y 1
    * @param varint, feature type, Variance 0 or Integral 1
    */
    double* calculateFeature(IplImage* input, int flag, int varorintegral);

    /**
    * Model Image Variance in X and Y
    */
    double* trackerModelFeatureVARIANCEX;
    double* trackerModelFeatureVARIANCEY;

    /**
    * Model Image Integral in X and Y
    */
    double* trackerModelFeatureINTEGRALX;
    double* trackerModelFeatureINTEGRALY;

    /**
    * Model Image Size in X and Y
    */
    int trackerModelFeatureSizeX;
    int trackerModelFeatureSizeY;

    /**
    * Run grid search
    *@param gray, input image for which the grid Search should be run on
    *@param size, Size of the feature
    *@param flag, direction, X 0 or Y 1
    *@param d, scale factor
    *@param e, translate factor
    *@param varianceImage, feature Variance of Image
    *@param integralImage, feature Integral of Image
    *@param anchor, experimental - currently not used
    */
    double runGridSearch(IplImage* gray, int size, int flag, double* d,
      double* e, double* varianceImage, double* integralImage, int anchor);

    /**
    * Find Parameters
    *@param scaleFactor, Last Updated Scale Factor
    *@param translateFactor, Last Updated Translate Factor
    *@param updatedScaleFactor, New Scale Factor
    *@param updatedTranslateFactor, New Translate Factor
    *@param feature, feature Variance of Image
    *@param feature1, feature Integral of Image
    *@param varianceImage, feature Variance of Model
    *@param integralImage, feature Integral of Model
    *@param size, Not Used
    *@param scale, Scale Factor
    *@param translate, Translate Factor
    *@param anchor, experimental - currently not used
    */
    double findParam(double scaleFactor, double translateFactor,
      double* updatedScaleFactor, double* updatedTranslateFactor,
      double* feature, double* feature1, double* featureModel,
      double* featureModel1, int size, double scale, double translate,
      int anchor);

   /**
    * Function used to find the difference between the feature of current
    * image and feature of model image
    *@param feature, Input Feature
    *@param featureModel, Model Feature
    *@param size, Size of Feature
    *@param px, Scale Factor
    *@param py, Translate Factor
    *@param anchor, experimental - currently not used
    */
    double difference(double* feature, double* featureModel, int size,
      double px, double py, int anchor);

    /**
    * Copy constructor
    * @param tracker, original value to copy
    */
    tracker(tracker&);

    /**
    * Assignment operator
    * @param tracker, original value to assign
    */
    tracker& operator =(const tracker&);

  public:
    /**
    * Error Value between last image from track call and truth image in Y
    */
    double lastDifference1;
    /**
    * Error Value between last image from track call and truth image in X
    */
    double lastDifference2;

    /**
    * Experimental - currently not used
    */
    CvPoint anchorPoint;

    /**
    * The Constuctor
    * Initializes internal variables
    */
    tracker();

    /**
    * The destructor
    */
    virtual ~tracker();

    /**
    * This function runs the tracker algorithm
    * @param input, Image on which tracker should run
    */
    void trackImage(IplImage* input);

    /**
    *This function correlates the points between input and output
    * @param p1, Input Point Co-ordinates on Model Image
    * @param p2, Output Point Co-ordinates on Current Image
    */
    void findPoint(CvPoint p1, CvPoint* p2);

    /**
    * Sets the Model (the truth) of tracking
    * @param input, truth image
    */
    void setModel(IplImage* input);
};

#endif // _INCL_GUARD_TRACK
