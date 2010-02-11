#ifndef _INCL_UTILS
#define _INCL_UTILS
/** @file */

#define MAX_THRESHOLD_LBP 14200
#define WIDTH_STEP_LBP 1400



/**
* Mace Filter
*/
typedef struct
{
    double thresholdPCER; /**< Peak Correlation Energy Ratio of MACE FILTER */
    int thresholdPSLR; /**< Peak to Side Lobe Ratio of MACE FILTER */
    char maceFilterName[300]; /**< Name XYZ.xml */
    CvMat *filter;
}mace;

/**
* Save the Mace Filter Struct
*@param maceFilter filter struct
*@param path Path String to Save
*/
void saveMace(mace * maceFilter,char *path);


/**
* LBP Diff ChiSquar Distance
*@param model Model Feature
*@param test Test Feature
*@result ChiSquare Diff
*/
double LBPdiff(    CvMat* model,    CvMat* test);
/**
* Check for Uniform Pattern
*@param i check Intensity Value
*@result 1 if its Uniform Patterns , 0 otherwise
*/

int checkBit(int i);
/**
* Check for Uniform Pattern
*@param img Image Input
*@param px X Co-ordinate
*@param py Y Co-ordinate
*@param threshold Threshold to Check
*@result 1 if its above , 0 otherwise
*/

double getBIT(IplImage* img,double px,double py,double threshold);

/**
* Create LBP Hist Feature
*@param img Image Input
*@param CvMat Features Final
*/
void  featureLBPHist(IplImage * img,CvMat *features_final);


/**
* Config Structure For the System
*/
typedef struct
{
double percentage;/**< Percentage of Threshold */
}config;



/**
* Create Gaussian Kernel
*@param size Size of Kernel
*/
CvMat * createGaussianFilter(int size);
/**
* Create Self Quotient Image with Specific kernel
*@param filter Gaussian Kernel
*@param image Image input
*@param size Size of Kernel
*@return SQI of Image
*/
IplImage * SQI(CvMat * filter,IplImage *  image,int size);
/**
* Create Self Quotient Image using kernel 3 5 9
*@param im Image input
*@param final Image output
*/
void createSQI(IplImage * im,IplImage *final);
/**
* if file exists
*@param filename File Name to Check
*@return 1 if True , 0 if False
*/
int file_exists(const char* filename);
/**
* Sets Config for the System
*@param configuration Configuration Structure
*@param configDirectory Location of Configuration Directory
@see config
*/
void setConfig(config *configuration,char * configDirectory);
/**
* Gets Config for the System
*@param configDirectory Location of Configuration Directory
@return Current Config
*/
config * getConfig(char *configDirectory);
/**
* Tranformation to log Of Image
*@param img Image input
*@param logImage Image output
*/
void logOfImage(IplImage * img,IplImage *logImage);
/**
* Tranformation to shift Quadrants to put (0,0) -  (center,center)
*@param src_arr Image input
*@param dst_arr Image output
*/
void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr );
/**
* Computes Mace ( Minimum Average Correlation Energy ) Filter for the Set of Images
*@param img Array of IplImages
*@param size Number of Images
*@param SIZE_OF_IMAGE Size of the Resized Image
*@result 2D Fourier Space Filter
*/
CvMat *computeMace(IplImage **img,int size,int  SIZE_OF_IMAGE);
/**
* Computes PSLR of Filter and Image
*@param maceFilterVisualize 2D Fourier Space Filter
*@param img Test Image
*@param SIZE_OF_IMAGE Size to Resize to
*@result peakToSideLobeRatio
*/
int peakToSideLobeRatio(CvMat*maceFilterVisualize,IplImage *img,int  SIZE_OF_IMAGE);

/**
* Computes peakCorrPlaneEnergy of Filter and Image
*@param maceFilterVisualize 2D Fourier Space Filter
*@param img Test Image
*@param SIZE_OF_IMAGE Size to Resize to
*@result peakCorrPlaneEnergy
*/
double peakCorrPlaneEnergy(CvMat*maceFilterVisualize,IplImage *img,int  SIZE_OF_IMAGE);

/**
* Rotates a Point By an angle , NOT USED -MIGHT BE WRONG WILL FIX LATER
*@param srcP Source Point
*@param dstP Destination Point
*@param angle Angle at which Point should be turned
*@param centreX Pivot Center X Co-ordinate
*@param centreY Pivot Center Y Co-ordinate
*/
void rotatePoint(CvPoint* srcP,CvPoint* dstP,double angle,float centreX, float centreY);
/**
* Rotates an Image By an Angle
*@param angle Angle at which Point should be turned
*@param centreX Pivot Center X Co-ordinate
*@param centreY Pivot Center Y Co-ordinate
*@param img Source Image
*@param dstimg Destination Image
*/
void rotate(double angle, float centreX, float centreY,IplImage * img,IplImage * dstimg);
/**
* Computes Center of Mass of Image in a Particular Direction
*@param src Source Image
*@param flagXY  direction, 0 -X , 1 -Y
*@return Co-ordinate
*/
double CenterofMass(IplImage* src,int flagXY);


#endif
