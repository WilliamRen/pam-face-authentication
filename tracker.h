#include "cv.h"

class tracker
{
public:
    tracker(void);
    void trackImage(IplImage * input);
    void findPoint(CvPoint p1,CvPoint *p2);
    void setModel(IplImage * input);
      double lastDifference1;
    double lastDifference2;
    CvPoint anchorPoint;

private:
    double stateVariableScaleX;
    double stateVariableTranslateX;
    double stateVariableScaleY;
    double stateVariableTranslateY;
    double lastImageWidth;
    double lastImageHeight;

    double * calculateFeature(IplImage * input,int flag,int varint);
    double *trackerModelFeatureVARIANCEX;
    double *trackerModelFeatureVARIANCEY;
    double *trackerModelFeatureINTEGRALX;
    double *trackerModelFeatureINTEGRALY;




    int trackerModelFeatureSizeX;
    int trackerModelFeatureSizeY;
    double runGridSearch(IplImage *gray,int size,int flag,double *d,double* e ,double * integralTemp,double * integralTemp1,int anchor);
    double gridSearch(double  scaleFactor,double translateFactor,double * updatedScaleFactor,double * updatedTranslateFactor,double *feature,double*feature1,double* featureModel,double* featureModel1,int size,double scale,double translate,int anchor);
    double difference(double * feature,double * featureModel,int size, double px,double py,int anchor);
};
