// NOT USED!!


/*
    Copyright (C) 2008 Rohan Anil (rohan.anil@gmail.com) , Alex Lau ( avengermojo@gmail.com)

    Google Summer of Code Program 2008
    Mentoring Organization: openSUSE


    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
    numberOfTrainingFaces = loadfaceImageArray("/etc/pam_face_authentication/facemanager/face.key");
    doPCA();
    projectedTrainFaceMat = cvCreateMat( numberOfTrainingFaces, numberOfEigenVectors, CV_32FC1 );
    offset = projectedTrainFaceMat->step / sizeof(float);
    for (i=0; i<numberOfTrainingFaces; i++)
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
    for (i=0; i<numberOfEigenVectors; i++)
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
    fileStorage = cvOpenFileStorage( "/etc/pam_face_authentication/facedata.xml", 0, CV_STORAGE_WRITE );
    cvWriteInt( fileStorage, "numberOfEigenVectors", numberOfEigenVectors );
    cvWriteInt( fileStorage, "numberOfTrainingFaces", numberOfTrainingFaces );
    cvWrite(fileStorage, "eigenValueMatrix", eigenValueMatrix, cvAttrList(0,0));
    cvWrite(fileStorage, "projectedTrainFaceMat", projectedTrainFaceMat, cvAttrList(0,0));
    cvWrite(fileStorage, "avgTrainImg", averageTrainingImage, cvAttrList(0,0));
    for (i=0; i<numberOfEigenVectors; i++)
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

    if ( !(file1 = fopen(filename, "r")) )
    {
        fprintf(stderr, "Can\'t open file %s\n", filename);
        return 0;
    }

    //while( fgets(userFile, 512, file1) )

    while (fscanf(file1,"%s %s", userName, userFile)!=EOF )
    {
        ++numberOfFaces;
    }
    rewind(file1);


    faceImageArray        = (IplImage **)cvAlloc( numberOfFaces*sizeof(IplImage *) );
    int  i;
    for (i=0; i<numberOfFaces; i++)
    {


        // strcat(userFile,"/etc/pam_face_authentication/facemanager/");

        fscanf(file1,"%s %s", userName, userFile);
        //   printf("%s ",userFile);

        faceImageArray[i] = cvLoadImage(userFile, CV_LOAD_IMAGE_GRAYSCALE);

        if ( !faceImageArray[i] )
        {
            fprintf(stderr, "Can\'t load image from %s\n", userFile);
            return 0;
        }
    }
    fclose(file1);
    /*
        faceImageArray[i] = cvLoadImage("/etc/pam_face_authentication/facemanager/facemanager1.pgm", CV_LOAD_IMAGE_GRAYSCALE);
        numberOfFaces++;
        i++;
        faceImageArray[i] = cvLoadImage("/etc/pam_face_authentication/facemanager/facemanager2.pgm", CV_LOAD_IMAGE_GRAYSCALE);
        numberOfFaces++;

    */
    return numberOfFaces;
}


