/*
    Copyright (C) 2008 Rohan Anil (rohan.anil@gmail.com) , Alex Lau ( avengermojo@gmail.com)

    Google Summer of Code Program 2008
    Mentoring Organization: openSUSE
    Mentor: Alex Lau

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
#include <stdio.h>
#include <string.h>
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include "pam_face_defines.h"


//// Global variables
IplImage * testFace        = 0; // array of face images
int numberOfTrainingFaces               = 0; // the number of training images
int numberOfEigenVectors                   = 0; // the number of eigenvalues
IplImage * averageTrainingImage       = 0; // the average image
IplImage ** eigenVectArr      = 0; // eigenvectors
CvMat * eigenValueMatrix           = 0; // eigenvalues
CvMat * projectedTrainFaceMat = 0; // projected training faces


//// Function prototypes

char recognize(char* username);
int  loadTrainingData();
int  findNearestNeighbor(float * projectedTestFace);

double findDifference(float * ,int );
int  loadFaceImgArray(char * filename);
int findIndex(char* username);
char* findUserName(int index);
char tempUserName[512];
char imgFilename[512];
double threshold=50000000;
char * fileNameImage(char *ch,char *value)
{
    char *fullPath;
    fullPath=(char *)calloc(  strlen(imgPath) + strlen(value)+strlen(imgExt)+2,sizeof(char));
    strcat(fullPath,imgPath);
    strcat(fullPath,value);
    strcat(fullPath,ch);
    strcat(fullPath,imgExt);
    return fullPath;
}
double valFind( IplImage*first, IplImage*second)
{
    IplImage* resultMatch = cvCreateImage( cvSize(1,1), IPL_DEPTH_32F, 1 );
    CvPoint    minloc, maxloc;
    minloc=cvPoint(0,0);
    maxloc=cvPoint(0,0);
    double		minval, maxval;
    cvMatchTemplate( first, second, resultMatch, CV_TM_SQDIFF );
    cvMinMaxLoc( resultMatch, &minval, &maxval, &minloc, &maxloc, 0 );

    return minval;

}
double faceSplitReturnVal( IplImage*first, IplImage*second)
{
    double d =valFind(first,second);
    IplImage* firstEye=cvCreateImage( cvSize(first->width,35), 8, first->nChannels );
    cvSetImageROI(first,cvRect(0,35,first->width,35));
    cvResize( first,firstEye, CV_INTER_LINEAR ) ;
    cvResetImageROI(first);

    IplImage* secondEye=cvCreateImage( cvSize(second->width,35), 8, first->nChannels );
    cvSetImageROI(second,cvRect(0,35,second->width,35));
    cvResize( second,secondEye, CV_INTER_LINEAR ) ;
    cvResetImageROI(second);



    d=d+(valFind(firstEye,secondEye)*2); // Eye area


    IplImage* firstEyebtw=cvCreateImage( cvSize(60,70), 8, first->nChannels );
    cvSetImageROI(first,cvRect(30,60,60,70));
    cvResize( first,firstEyebtw, CV_INTER_LINEAR ) ;
    cvResetImageROI(first);

    IplImage* secondEyebtw=cvCreateImage( cvSize(60,70), 8, first->nChannels );
    cvSetImageROI(second,cvRect(30,60,60,70));
    cvResize( second,secondEyebtw, CV_INTER_LINEAR ) ;
    cvResetImageROI(second);


    d=d+(valFind(firstEyebtw,secondEyebtw)*1.6); // between eye area
    return d;

}



char recognize(char* username)
{
    char* userFile;
    userFile=(char *)calloc(strlen(username) + 18,sizeof(char));
    strcat(userFile,"/lib/pamface/");;
    strcat(userFile,username);
    strcat(userFile,".pgm");
    int i;      // the number of test images
    CvMat * trainPersonNumMat = 0;  // the person numbers during training
    float * projectedTestFace = 0;

    testFace = cvLoadImage(userFile, CV_LOAD_IMAGE_GRAYSCALE);
    // load the saved training data
    if ( !loadTrainingData()) return;
    // project the test images onto the PCA subspace
    projectedTestFace = (float *)cvAlloc( numberOfEigenVectors*sizeof(float) );
    int index, nearest;
    char* userNameReturn;
    // project the test image onto the PCA subspace
    cvEigenDecomposite(testFace,numberOfEigenVectors,eigenVectArr,0, 0,averageTrainingImage,projectedTestFace);

    if (findIndex(username)!=-1)
    {
        double val;

        IplImage* image1=cvLoadImage(fileNameImage("1",username), CV_LOAD_IMAGE_GRAYSCALE );
        IplImage* image2=cvLoadImage(fileNameImage("2",username), CV_LOAD_IMAGE_GRAYSCALE );
        IplImage* image3=cvLoadImage(fileNameImage("3",username), CV_LOAD_IMAGE_GRAYSCALE );
        IplImage* image4=cvLoadImage(fileNameImage("4",username), CV_LOAD_IMAGE_GRAYSCALE );
        IplImage* image5=cvLoadImage(fileNameImage("5",username), CV_LOAD_IMAGE_GRAYSCALE );
        IplImage* image6=cvLoadImage(fileNameImage("6",username), CV_LOAD_IMAGE_GRAYSCALE );
        double averageValueThreshold=0;
        /*
        averageValueThreshold+=faceSplitReturnVal(image1,image2);
        averageValueThreshold+=faceSplitReturnVal(image1,image3);
        averageValueThreshold+=faceSplitReturnVal(image1,image4);
        averageValueThreshold+=faceSplitReturnVal(image1,image5);
        averageValueThreshold+=faceSplitReturnVal(image1,image6);
        averageValueThreshold+=faceSplitReturnVal(image2,image3);
        averageValueThreshold+=faceSplitReturnVal(image2,image4);
        averageValueThreshold+=faceSplitReturnVal(image2,image5);
        averageValueThreshold+=faceSplitReturnVal(image2,image6);
        averageValueThreshold+=faceSplitReturnVal(image3,image4);
        averageValueThreshold+=faceSplitReturnVal(image3,image5);
        averageValueThreshold+=faceSplitReturnVal(image3,image6);
        averageValueThreshold+=faceSplitReturnVal(image4,image5);
        averageValueThreshold+=faceSplitReturnVal(image4,image6);
        averageValueThreshold+=faceSplitReturnVal(image5,image6);
        averageValueThreshold=averageValueThreshold/15;
        printf("average value %e \n",averageValueThreshold);
        */
        double valTestFace=0;
        valTestFace=faceSplitReturnVal(image1,testFace);
        if (valTestFace<threshold)
            return 'y';
        valTestFace=faceSplitReturnVal(image2,testFace);
        if (valTestFace<threshold)
            return 'y';
        valTestFace=faceSplitReturnVal(image3,testFace);
        if (valTestFace<threshold)
            return 'y';
        valTestFace=faceSplitReturnVal(image4,testFace);
        if (valTestFace<threshold)
            return 'y';
        valTestFace=faceSplitReturnVal(image5,testFace);
        if (valTestFace<threshold)
            return 'y';
        valTestFace=faceSplitReturnVal(image6,testFace);
        if (valTestFace<threshold)
            return 'y';



        /*
        printf("Value = %e \n",faceSplitReturnVal(image1,testFace));
          printf("Value = %e \n",faceSplitReturnVal(image2,testFace));
          printf("Value = %e \n",faceSplitReturnVal(image3,testFace));
          printf("Value = %e \n",faceSplitReturnVal(image4,testFace));
          val=findDifference(projectedTestFace,findIndex(username));
          printf("Value 1=%e \n",val);
          val=findDifference(projectedTestFace,findIndex(username)+1);
          printf("Value 2=%e \n",val+1);
          val=findDifference(projectedTestFace,findIndex(username)+2);
          printf("Value 3=%e \n",val+2);
          val=findDifference(projectedTestFace,findIndex(username)+3);
          printf("Value 4=%e \n",val+3);
          val=findDifference(projectedTestFace,findIndex(username)+4);
          printf("Value 5=%e \n",val+4);
          val=findDifference(projectedTestFace,findIndex(username)+5);
          printf("Value 6=%e \n",val+5);

          */
        //  if (val<threshold)
        //return 'y';
        //else
        //  return 'n';
//printf("%e \n",val);
    }
    else
        return 'n';

    /*
       index = findNearestNeighbor(projectedTestFace);
       //// stupid hack  for getting PCA running

       if ((index!=numberOfTrainingFaces-1)&&(index!=numberOfTrainingFaces-2))
       {
           userNameReturn = findUserName(index);
           if (!strcmp(userNameReturn,username))
           {
               return 'y';
           }
           else
           {
               return 'n';
           }

       }
       else
       {
           return 'n';
       }

       */

}
int findIndex(char* username)
{
    FILE *fileKey;
    if ( !(fileKey = fopen("/lib/pamface/facemanager/face.key", "r")) )
    {
        fprintf(stderr, "Error Occurred Accessing Key File\n");
        return 0;
    }
    int numberOfFaces=0;
    while (fscanf(fileKey,"%s %s", tempUserName, imgFilename)!=EOF )
    {
        ++numberOfFaces;
    }
    rewind(fileKey);
    int i;
    for (i=0;i<numberOfFaces;i++)
    {
        fscanf(fileKey,"%s %s", tempUserName, imgFilename);
        if (strcmp(tempUserName,username)==0)
            return i;

    }

    fclose(fileKey);
    return -1;
}

char* findUserName(int index)
{

    FILE *fileKey;
    if ( !(fileKey = fopen("/lib/pamface/facemanager/face.key", "r")) )
    {
        fprintf(stderr, "Error Occurred Accessing Key File\n");
        return 0;
    }

    int i;
    for (i=0;i<=index;i++)
        fscanf(fileKey,"%s %s", tempUserName, imgFilename);

    fclose(fileKey);
    return tempUserName;
}

int findNearestNeighbor(float * projectedTestFace)
{

    double leastDistSq = DBL_MAX;
    int i,k, index;

    for ( k=0; k<numberOfTrainingFaces; k++)
    {
        double distSq=0;
        for (i=0; i<numberOfEigenVectors; i++)
        {

            float d_i =	projectedTestFace[i] -	projectedTrainFaceMat->data.fl[k*numberOfEigenVectors + i];
            distSq += (d_i*d_i)/ eigenValueMatrix->data.fl[i];  // Mahalanobis
            //	distSq += d_i*d_i;/
            //printf("%f \n",projectedTrainFaceMat->data.fl[k*numberOfEigenVectors + i]);
        }



        //printf("%f \n",distSq);
        if (distSq < leastDistSq)
        {
            leastDistSq = distSq;
            index = k;
        }
    }

    return index;
}



double findDifference(float * projectedTestFace,int indexFace)
{
    int i,k, index;
    k=indexFace;
    double distSq=0;
    for (i=0; i<numberOfEigenVectors; i++)
    {
        float d_i =	projectedTestFace[i] -	projectedTrainFaceMat->data.fl[k*numberOfEigenVectors + i];
        distSq += (d_i*d_i)/ eigenValueMatrix->data.fl[i];
    }
    return distSq;
}

int loadTrainingData()
{
    CvFileStorage * fileStorage;
    int i;

    // create a file-storage interface
    fileStorage = cvOpenFileStorage( "/lib/pamface/facedata.xml", 0, CV_STORAGE_READ );
    if ( !fileStorage )
    {
        fprintf(stderr, "Can't open facedata.xml\n");
        return 0;
    }

    numberOfEigenVectors = cvReadIntByName(fileStorage, 0, "numberOfEigenVectors", 0);
    numberOfTrainingFaces = cvReadIntByName(fileStorage, 0, "numberOfTrainingFaces", 0);
    eigenValueMatrix  = (CvMat *)cvReadByName(fileStorage, 0, "eigenValueMatrix", 0);
    projectedTrainFaceMat = (CvMat *)cvReadByName(fileStorage, 0, "projectedTrainFaceMat", 0);
    averageTrainingImage = (IplImage *)cvReadByName(fileStorage, 0, "avgTrainImg", 0);
    eigenVectArr = (IplImage **)cvAlloc(numberOfTrainingFaces*sizeof(IplImage *));
    for (i=0; i<numberOfEigenVectors; i++)
    {
        char varname[200];
        sprintf( varname, "eigenVect_%d", i );
        eigenVectArr[i] = (IplImage *)cvReadByName(fileStorage, 0, varname, 0);
    }

    // release the file-storage interface
    cvReleaseFileStorage( &fileStorage );

    return 1;
}
