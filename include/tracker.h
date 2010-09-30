/** @file */
#include "cv.h"
#ifndef _INCL_GUARD_TRACK
#define _INCL_GUARD_TRACK
/**
* Tracker Class. This Class implements a Experimental Object Tracker, I use Variance and Integral Projection in X and Y directions as Features and uses Grid Search Algorithm To Fit the Model Feature to The input Feature.
*/
class tracker
{
public:
    /**
    *The Constuctor
    *Initializes internal variables
    */
    tracker(void);
    /**
    *This is functions runs the Tracker Algorithm
    *@param input Image on which Tracker Should Run
    */
    void trackImage(IplImage * input);

    /**
    *This is functions runs the Tracker Algorithm
    *@param p1 Input Point Co-ordinates on Model Image
    *@param p2 Output Point Co-ordinates on Current Image
    */
    void findPoint(CvPoint p1,CvPoint *p2);


    /**
    *Sets the Model (The Truth) of Tracking
    *@param input Truth Image
    */
    void setModel(IplImage * input);


    /**
    *Error Value between Last Image from Track Call and Truth Image in Y
    */
    double lastDifference1;
    /**
    *Error Value between Last Image from Track Call and Truth Image in X
    */
    double lastDifference2;
    /**
    *Experimental , Not used
    */
    CvPoint anchorPoint;

private:

    /**
    * Scale in X   - Scale*(Co-ordinate) + Translate
    */
    double stateVariableScaleX;
    /**
    * Translate in X   - Scale*(Co-ordinate) + Translate
    */
    double stateVariableTranslateX;
    /**
    * Scale in Y   - Scale*(Co-ordinate) + Translate
    */
    double stateVariableScaleY;
    /**
    * Translate in Y   - Scale*(Co-ordinate) + Translate
    */
    double stateVariableTranslateY;

    /**
    * Image Width of Last Track Image Call
    */
    double lastImageWidth;
    /**
    * Image Height of Last Track Image Call
    */
    double lastImageHeight;

    /**
    * Calculate the Features
    *@param input input image for which the feature should be calculated
    *@param flag Direction, X 0 or Y 1
    *@param varint Feature Type, Variance 0 or Integral 1
    */
    double * calculateFeature(IplImage * input,int flag,int varint);

    /**
    * Model Image Variance in X
    */
    double *trackerModelFeatureVARIANCEX;
    /**
    * Model Image Variance in Y
    */
    double *trackerModelFeatureVARIANCEY;
    /**
    * Model Image Integral in X
    */
    double *trackerModelFeatureINTEGRALX;
    /**
    * Model Image Integral in Y
    */
    double *trackerModelFeatureINTEGRALY;

    /**
    * Model Image Size in X
    */
    int trackerModelFeatureSizeX;
    /**
    * Model Image Size in Y
    */
    int trackerModelFeatureSizeY;

    /**
    * run Grid Search
    *@param gray input image for which the grid Search should be run on
    *@param size Size of the Feature
    *@param flag Direction, X 0 or Y 1
    *@param d Scale Factor
    *@param e Translate Factor
    *@param varianceImage feature Variance of Image
    *@param integralImage feature Integral of Image
    *@param anchor Experimental Not Used
    */
    double runGridSearch(IplImage *gray,int size,int flag,double *d,double* e ,double * varianceImage,double * integralImage,int anchor);
    /**
    * Find Paramaters
    *@param scaleFactor Last Updated Scale Factor
    *@param translateFactor Last Updated Translate Factor
    *@param updatedScaleFactor New Scale Factor
    *@param updatedTranslateFactor New Translate Factor
    *@param feature feature Variance of Image
    *@param feature1 feature Integral of Image
    *@param varianceImage feature Variance of Model
    *@param integralImage feature Integral of Model
    *@param size Not Used =P
    *@param scale Scale Factor
    *@param translate Translate Factor
    *@param anchor Experimental Not Used
    */
    double findParam(double  scaleFactor,double translateFactor,double * updatedScaleFactor,double * updatedTranslateFactor,double *feature,double*feature1,double* featureModel,double* featureModel1,int size,double scale,double translate,int anchor);
   /**
    * Function used to find Difference between feature of current image and feature of model image
    *@param feature Input Feature
    *@param featureModel Model Feature
    *@param size Size of Feature
    *@param px Scale Factor
    *@param py Translate Factor
    *@param anchor Experimental Not Used
    */
    double difference(double * feature,double * featureModel,int size, double px,double py,int anchor);
};
#endif
