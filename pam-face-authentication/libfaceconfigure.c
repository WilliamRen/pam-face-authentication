
#include "highgui.h"
#include "cv.h"
#include "cvaux.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>



char* userNameGlobal;

//// Global variables
IplImage ** faceImageArray        = 0; // array of face images

int numberOfTrainingFaces               = 0; // the number of training images
int numberOfEigenVectors                   = 0; // the number of eigenvalues
IplImage * averageTrainingImage       = 0; // the average image
IplImage ** eigenVectorArray      = 0; // eigenvectors
CvMat * eigenValueMatrix           = 0; // eigenvalues
CvMat * projectedTrainFaceMat = 0; // projected training faces
//// Function prototypes
void learn();
void doPCA();
void storeTrainingData();
int  loadfaceImageArray(char * filename);


void learn()
{
	int i, offset;
	numberOfTrainingFaces = loadfaceImageArray("/lib/pamface/facemanager/face.key");
	doPCA();
	projectedTrainFaceMat = cvCreateMat( numberOfTrainingFaces, numberOfEigenVectors, CV_32FC1 );
	offset = projectedTrainFaceMat->step / sizeof(float);
	for(i=0; i<numberOfTrainingFaces; i++)
	{
		//int offset = i * numberOfEigenVectors;
		cvEigenDecomposite(
			faceImageArray[i],
			numberOfEigenVectors,
			eigenVectorArray,
			0, 0,
			averageTrainingImage,
			//projectedTrainFaceMat->data.fl + i*numberOfEigenVectors);
			projectedTrainFaceMat->data.fl + i*offset);
	}

	storeTrainingData();
}



void doPCA()
{
	int i;
	CvTermCriteria calcLimit;
	CvSize faceImgSize;
    numberOfEigenVectors = numberOfTrainingFaces-1;
	faceImgSize.width  = faceImageArray[0]->width;
	faceImgSize.height = faceImageArray[0]->height;
	eigenVectorArray = (IplImage**)cvAlloc(sizeof(IplImage*) * numberOfEigenVectors);
	for(i=0; i<numberOfEigenVectors; i++)
		eigenVectorArray[i] = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);
	eigenValueMatrix = cvCreateMat( 1, numberOfEigenVectors, CV_32FC1 );
	averageTrainingImage = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);
	calcLimit = cvTermCriteria( CV_TERMCRIT_ITER, numberOfEigenVectors, 1);
	cvCalcEigenObjects(
		numberOfTrainingFaces,
		(void*)faceImageArray,
		(void*)eigenVectorArray,
		CV_EIGOBJ_NO_CALLBACK,
		0,
		0,
		&calcLimit,
		averageTrainingImage,
		eigenValueMatrix->data.fl);
	cvNormalize(eigenValueMatrix, eigenValueMatrix, 1, 0, CV_L1, 0);
}






void storeTrainingData()
{
	CvFileStorage * fileStorage;
	int i;
    fileStorage = cvOpenFileStorage( "/lib/pamface/facedata.xml", 0, CV_STORAGE_WRITE );
	cvWriteInt( fileStorage, "numberOfEigenVectors", numberOfEigenVectors );
	cvWriteInt( fileStorage, "numberOfTrainingFaces", numberOfTrainingFaces );
	cvWrite(fileStorage, "eigenValueMatrix", eigenValueMatrix, cvAttrList(0,0));
	cvWrite(fileStorage, "projectedTrainFaceMat", projectedTrainFaceMat, cvAttrList(0,0));
	cvWrite(fileStorage, "avgTrainImg", averageTrainingImage, cvAttrList(0,0));
	for(i=0; i<numberOfEigenVectors; i++)
	{
		char varname[200];
		sprintf( varname, "eigenVect_%d", i );
		cvWrite(fileStorage, varname, eigenVectorArray[i], cvAttrList(0,0));
	}

	cvReleaseFileStorage( &fileStorage );
}


int loadfaceImageArray(char * filename)
{

	FILE * file1 = 0;
	char  userName[512];
	char   userFile[512];
	int  numberOfFaces=0;

	if( !(file1 = fopen(filename, "r")) )
	{
		fprintf(stderr, "Can\'t open file %s\n", filename);
		return 0;
	}

	//while( fgets(userFile, 512, file1) )

	while(fscanf(file1,"%s %s", userName, userFile)!=EOF )
	{++numberOfFaces;}
	rewind(file1);


	faceImageArray        = (IplImage **)cvAlloc( numberOfFaces*sizeof(IplImage *) );
	int  i;
	for(i=0; i<numberOfFaces; i++)
	{


  // strcat(userFile,"/lib/pamface/facemanager/");

   	fscanf(file1,"%s %s", userName, userFile);
printf("%s ",userFile);

		faceImageArray[i] = cvLoadImage(userFile, CV_LOAD_IMAGE_GRAYSCALE);

		if( !faceImageArray[i] )
		{
			fprintf(stderr, "Can\'t load image from %s\n", userFile);
			return 0;
		}
	}
	fclose(file1);

faceImageArray[i] = cvLoadImage("/lib/pamface/facemanager/facemanager1.pgm", CV_LOAD_IMAGE_GRAYSCALE);
numberOfFaces++;
i++;
faceImageArray[i] = cvLoadImage("/lib/pamface/facemanager/facemanager2.pgm", CV_LOAD_IMAGE_GRAYSCALE);
numberOfFaces++;
			return numberOfFaces;
	}


