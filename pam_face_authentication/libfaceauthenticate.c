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

char recognize(int *an,char* username,int *percentage);
int  loadTrainingData();
int  findNearestNeighbor(float * projectedTestFace);

double findDifference(float * ,int );
int  loadFaceImgArray(char * filename);
int findIndex(char* username);
char* findUserName(int index);
char tempUserName[512];
char imgFilename[512];
double threshold=3259;
double threshold2=235000000;
double hist1[256];
double hist2[256];
double currentVal;

char * fileNameImage(char *ch,char *value)
{
    char *fullPath;
    fullPath=(char *)calloc(  strlen(imgPath) + strlen(value)+strlen(imgExt)+1 +strlen(ch),sizeof(char));
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
    cvReleaseImage( &resultMatch);
    return minval;

}
double faceSplitReturnVal( IplImage*first, IplImage*second)
{

    IplImage* firstHalf1=cvCreateImage( cvSize(first->width/2,first->height), 8, first->nChannels );
    cvSetImageROI(first,cvRect(0,0,first->width/2,first->height));
    cvResize( first,firstHalf1, CV_INTER_LINEAR ) ;
    cvResetImageROI(first);

    IplImage* secondHalf1=cvCreateImage( cvSize(second->width/2,second->height), 8, first->nChannels );
    cvSetImageROI(second,cvRect(0,0,second->width/2,second->height));
    cvResize( second,secondHalf1, CV_INTER_LINEAR ) ;
    cvResetImageROI(second);

    IplImage* firstHalf2=cvCreateImage( cvSize(first->width/2,first->height), 8, first->nChannels );
    cvSetImageROI(first,cvRect(first->width/2,0,first->width/2,first->height));
    cvResize( first,firstHalf2, CV_INTER_LINEAR ) ;
    cvResetImageROI(first);

    IplImage* secondHalf2=cvCreateImage( cvSize(second->width/2,second->height), 8, first->nChannels );
    cvSetImageROI(second,cvRect(second->width/2,0,second->width/2,second->height));
    cvResize( second,secondHalf2, CV_INTER_LINEAR ) ;
    cvResetImageROI(second);

    double d1 =valFind(firstHalf1,secondHalf1);
    double d2 =valFind(firstHalf2,secondHalf2);
    double d=0;
    if (d1<d2)
        d=d1*2;
    else
        d=d2*2;
//   double d =valFind(first,second);
    cvReleaseImage( &firstHalf1);
    cvReleaseImage( &firstHalf2);

    cvReleaseImage( &secondHalf1);
    cvReleaseImage( &secondHalf2);

    IplImage* firstEye=cvCreateImage( cvSize(first->width,40), 8, first->nChannels );
    cvSetImageROI(first,cvRect(0,25,first->width,65));
    cvResize( first,firstEye, CV_INTER_LINEAR ) ;
    cvResetImageROI(first);

    IplImage* secondEye=cvCreateImage( cvSize(second->width,40), 8, first->nChannels );
    cvSetImageROI(second,cvRect(0,25,second->width,65));
    cvResize( second,secondEye, CV_INTER_LINEAR ) ;
    cvResetImageROI(second);


    d=d+(valFind(firstEye,secondEye)*2); // Eye area
    cvReleaseImage( &firstEye);
    cvReleaseImage( &secondEye);
    return d;

}
/*
double getBIT(IplImage* img,double px,double py,double threshold)
{
    if (px<0 || py<0 || px>=img->width || py>=img->height)
        return 0;
    else
    {
        CvScalar s;
        s=cvGet2D(img,py,px);
        if (s.val[0]>=threshold)
            return 1;
        else
            return 0;
    }
}
*/

/*
int checkBit(int i)
{
    int j=i;
    int bit8=(i%2);
    int bit7=((i/2)%2);
    int bit6=((i/4)%2);
    int bit5=((i/8)%2);
    int bit4=((i/16)%2);
    int bit3=((i/32)%2);
    int bit2=((i/64)%2);
    int bit1=((i/128)%2);
    // printf("%d %d %d %d %d %d %d %d \n",bit1,bit2,bit3,bit4,bit5,bit6,bit7,bit8);
    int bitVector[9]  =   {bit1,bit8,bit7, bit6, bit5,bit4, bit3,bit2,bit1};

    int current=bitVector[0];
    int count=0;
    for (i=0;i<9;i++)
    {
        if (current!=bitVector[i])
            count++;
        current=bitVector[i];
    }
    if (count>2)
        return -1;
    else
        return 0;
}
*/
double histDifference(IplImage* img1,IplImage* img2)
{

    int i,j;
    int m;
    for (i=0;i<256;i++)
    {
        hist1[i]=0;
        hist2[i]=0;
    }

    for (i=0;i<20;i++)
    {

        for (j=0;j<20;j++)
        {
            CvScalar s;
            s=cvGet2D(img1,i,j);
            m=s.val[0];
            hist1[m]++;
        }
    }
    for (i=0;i<20;i++)
    {

        for (j=0;j<20;j++)
        {
            CvScalar s;
            s=cvGet2D(img2,i,j);
            m=s.val[0];
            hist2[m]++;
        }
    }


    double chiSquare1=0;
    double chiSquare=0;
    double lastBin1=0;
    double lastBin2=0;
    for (i=0;i<256;i++)
    {
        if (checkBit(i)!=-1)
        {
            chiSquare1=0;
            if ((hist1[i]+hist2[i])!=0)
                chiSquare1=((pow(hist1[i]-hist2[i],2)/(hist1[i]+hist2[i])));

            //     printf("%svn checkout https://pam-face-authentication.googlecode.com/svn/trunk/ pam-face-authentication --username rohan.anile \n ",chiSquare1);
            chiSquare=chiSquare1+chiSquare;
        }
        else
        {
            lastBin1+=hist1[i];
            lastBin2+=hist2[i];

        }

    }
    if ((lastBin1+lastBin2)!=0)
        chiSquare1=((pow(lastBin1-lastBin2,2)/(lastBin1+lastBin2)));
    //     printf("%e \n ",chiSquare1);
    chiSquare=chiSquare1+chiSquare;
    return chiSquare;

}
void createLBP(char * fullpath1,char * fullpath2)
{
    int i=0;
    int j=0;

    IplImage* image1=cvLoadImage(fullpath1, CV_LOAD_IMAGE_GRAYSCALE );
    IplImage* image1T=cvCreateImage( cvSize(image1->width,image1->height), 8, image1->nChannels );
    cvZero(image1T);
    for (i=0;i<image1->height;i++)
    {

        for (j=0;j<image1->width;j++)
        {
            int p1x,p2x,p3x,p4x,p5x,p6x,p7x,p8x;
            int p1y,p2y,p3y,p4y,p5y,p6y,p7y,p8y;

            p1x=j-1;
            p1y=i-1;

            p2x=j;
            p2y=i-1;

            p3x=j+1;
            p3y=i-1;

            p4x=j+1;
            p4y=i;

            p5x=j+1;
            p5y=i+1;

            p6x=j;
            p6y=i+1;

            p7x=j-1;
            p7y=i+1;


            p8x=j-1;
            p8y=i;

            CvScalar s;
            s=cvGet2D(image1,i,j);
            double bit1=128*getBIT(image1,p1x,p1y,s.val[0]);
            double bit2=64*getBIT(image1,p2x,p2y,s.val[0]);
            double bit3=32*getBIT(image1,p3x,p3y,s.val[0]);
            double bit4=16*getBIT(image1,p4x,p4y,s.val[0]);
            double bit5=8*getBIT(image1,p5x,p5y,s.val[0]);
            double bit6=4*getBIT(image1,p6x,p6y,s.val[0]);
            double bit7=2*getBIT(image1,p7x,p7y,s.val[0]);
            double bit8=1*getBIT(image1,p8x,p8y,s.val[0]);
            CvScalar s1;
            s1.val[0]=bit1+bit2+bit3+bit4+bit5+bit6+bit7+bit8;
            s1.val[1]=0;
            s1.val[2]=0;

            cvSet2D(image1T,i,j,s1);
        }
    }
    cvSaveImage(fullpath2,image1T);
    cvReleaseImage( &image1);
    cvReleaseImage( &image1T);
}
double LBPdiff(    IplImage* image1,    IplImage* image2)

{
    /* double weights[7][6]  =
     {
         {.2, .2, .2, .2, .2,.2},
         { 1,  1.3, svn checkout https://pam-face-authentication.googlecode.com/svn/trunk/ pam-face-authentication --username rohan.anil
    1.5,  1.5,  1.3, 1},
         { 1,  2,  1.7, 1.7,  2, 1},
         {.5,  1.1, 1.3,  1.3,  1.1,.5},
         {.3,  1,  1.4,  1.4,  1,.3},
         {.3,  1,  1.4, 1.4,  1, .3},
         {0, .5, .5, .5, .5,0}
     }; */
    double weights[7][4]  =
    {
        {.2, .2, .2, .2},
        { 1,  1.5,1.5,1},
        { 1,  2,  2,    1},
        {.5,  1.2, 1.2,.5},
        {.3,  1.2,  1.2, .3},
        {.3,  1.1,  1.1,  .3},
        {.1,  .5,       1, .1 }
    };

    int m,n;
    double differenceValue=0;
    for (m=0;m<4;m++)
    {
        for (n=0;n<7;n++)
        {

            cvSetImageROI(image1,cvRect((30*m),(n*20),30,20));
            cvSetImageROI(image2,cvRect((30*m),(n*20),30,20));
            //     printf ("%e , ",weights[n][m]);

            //cvResize( image2,subWindow1, CV_INTER_LINEAR ) ;
            differenceValue+=weights[n][m]*histDifference(image1,image2);
            cvResetImageROI(image1);
            cvResetImageROI(image2);

            //   cvResize( image2,subWindow1, CV_INTER_LINEAR ) ;
        }

    }
    return differenceValue;
}



char recognize(int *userid,char* username,int* percentage)
{
    /*
    char* userFile;

    userFile=(char *)calloc(strlen(username) + strlen(path)+strlen(imgExt)+1,sizeof(char));
    strcat(userFile,path);;
    strcat(userFile,username);
    strcat(userFile,imgExt);
    */

    char* userFile[300];
    sprintf(userFile,"/etc/pam-face-authentication/%s.pgm",username);

    IplImage * img = cvLoadImage(userFile,0);
    printf("%s \n",userFile);

    char temp[200];

    FILE *f1 =fopen("/etc/pam-face-authentication/testFeaturesDCT","w");
    FILE *f2 =fopen("/etc/pam-face-authentication/testFeaturesLBP","w");

    int Nx = floor((img->width - 4)/4);
    int Ny= floor((img->height - 4)/4);
    float* features = (float*)malloc(  Nx*Ny * 18 * sizeof(float) );


    featureDctMod2(img,features);

    int Nx1 = floor((img->width - 10)/10);
    int Ny1= floor((img->height - 10)/10);
    float* featuresLBP = (float*)malloc(  Nx1*Ny1 * 59 * sizeof(float) );
    featureLBPHist(img,featuresLBP);

    int i,j,k=0;
    sprintf(temp,"%d",getuid()); //uid doesnt matter over here

    fputs(temp,f1);
    fputs(" ",f1);
    fputs(temp,f2);
    fputs(" ",f2);
    for (i=0;i<Ny1;i++)
    {
        for (j=0;j<Nx1;j++)
        {
            for (k=0;k<59;k++)
            {
                sprintf(temp,"%d", (i*59*Nx1 + j*59 +k));
                fputs(temp,f2);
                fputs(":",f2);
                sprintf(temp,"%f",featuresLBP[i*59*Nx1 + j*59 +k]);
                fputs(temp,f2);
                fputs(" ",f2);
            }
        }
    }

    for (i=0;i<Ny;i++)
    {
        for (j=0;j<Nx;j++)
        {
            for (k=0;k<6;k++)
            {
                sprintf(temp,"%d", (i*18*Nx + j*18 +k));
                fputs(temp,f1);
                fputs(":",f1);
                sprintf(temp,"%f",features[i*18*Nx + j*18 +k]);
                fputs(temp,f1);
                fputs(" ",f1);
            }

        }
    }
    fputs("\n",f1);
    fputs("\n",f2);
    fclose(f1);
    fclose(f2);
    //printf(" \n 1testing");
    char* argv1[4]={"svm-scale","-r","/etc/pam-face-authentication/featuresDCT.range","/etc/pam-face-authentication/testFeaturesDCT"};
    FILE *fp5=fopen("/etc/pam-face-authentication/testFeaturesDCT.scale","w");
//    printf(" \n 2testing");

    svmScale(4,argv1,fp5);

    char* argv2[4]={"svm-scale","-r","/etc/pam-face-authentication/featuresLBP.range","/etc/pam-face-authentication/testFeaturesLBP"};
    FILE *fp6=fopen("/etc/pam-face-authentication/testFeaturesLBP.scale","w");
    svmScale(4,argv2,fp6);
    //printf("testing");

    system(BINDIR "/svm-predict -b 1 /etc/pam-face-authentication/testFeaturesDCT.scale /etc/pam-face-authentication/featuresDCT.scale.model /etc/pam-face-authentication/prediction");
    int ans;
    double sum=0;
    int ansMatch=-1;
    int login=1;
    double percentage1;
    parseSvmPrediction(&ans,&percentage1);
    ansMatch=ans;
    sum+=percentage1;
    printf("Answer %d \n",ans);
    printf("DCT Percent %e \n",percentage1);
    if (percentage1<.75)
        login=-1;

         printf("Login %d \n",login);
    // printf("Answer %d Percentage %e DCT \n",ans,percentage1);
    system(BINDIR "/svm-predict -b 1 /etc/pam-face-authentication/testFeaturesLBP.scale /etc/pam-face-authentication/featuresLBP.scale.model /etc/pam-face-authentication/prediction");
    parseSvmPrediction(&ans,&percentage1);
    printf("Answer %d \n",ans);
    printf("LBP Percent %e \n",percentage1);
    if (ansMatch==ans)
      {
            sum+=percentage1;
       *userid=ans;
      }
  printf("SUM Percent %e \n",sum);
    if (login!=-1)
    {
        if (percentage1<.75)
            login=-1;
    }
        printf("Login %d \n",login);
    if (sum<1.60)
    {
        login=-1;
    }

    printf("Login %d \n",login);
    // printf("Answer %d Percentage %e LBP \n",ans,percentage1);

    if (login==-1)
        return 'n';
    else
        return 'y';

    /*
        char* userFile;
        userFile=(char *)calloc(strlen(username) + strlen(path)+strlen(imgExt)+1,sizeof(char));
        char* userFileAKS;
        userFileAKS=(char *)calloc(strlen(username) + strlen(path)+strlen(imgExt)+5,sizeof(char));

        strcat(userFile,path);;
        strcat(userFile,username);
        strcat(userFile,imgExt);
        strcat(userFileAKS,path);;
        strcat(userFileAKS,username);
        strcat(userFileAKS,"_AKS");
        strcat(userFileAKS,imgExt);
        int i;
        createLBP(userFile,userFileAKS);
        testFace = cvLoadImage(userFileAKS, CV_LOAD_IMAGE_GRAYSCALE);
        int index, nearest;
        char* userNameReturn;
        if (findIndex(username)!=-1)
        {
            double val;

            double averageValueThreshold=0;

            createLBP(fileNameImage("1",username),fileNameImage("1_AKS",username));
            createLBP(fileNameImage("2",username),fileNameImage("2_AKS",username));
            createLBP(fileNameImage("3",username),fileNameImage("3_AKS",username));
            createLBP(fileNameImage("4",username),fileNameImage("4_AKS",username));
            createLBP(fileNameImage("5",username),fileNameImage("5_AKS",username));
            createLBP(fileNameImage("6",username),fileNameImage("6_AKS",username));

            IplImage* image1=cvLoadImage(fileNameImage("1_AKS",username), CV_LOAD_IMAGE_GRAYSCALE );
            IplImage* image2=cvLoadImage(fileNameImage("2_AKS",username), CV_LOAD_IMAGE_GRAYSCALE );
            IplImage* image3=cvLoadImage(fileNameImage("3_AKS",username), CV_LOAD_IMAGE_GRAYSCALE );
            IplImage* image4=cvLoadImage(fileNameImage("4_AKS",username), CV_LOAD_IMAGE_GRAYSCALE );
            IplImage* image5=cvLoadImage(fileNameImage("5_AKS",username), CV_LOAD_IMAGE_GRAYSCALE );
            IplImage* image6=cvLoadImage(fileNameImage("6_AKS",username), CV_LOAD_IMAGE_GRAYSCALE );

            double valTestFace[6];
            valTestFace[0]=LBPdiff(image1,testFace);
            valTestFace[1]=LBPdiff(image2,testFace);
            valTestFace[2]=LBPdiff(image3,testFace);
            valTestFace[3]=LBPdiff(image4,testFace);
            valTestFace[4]=LBPdiff(image5,testFace);
            valTestFace[5]=LBPdiff(image6,testFace);
            int m;
            int k=-1;
            int diff = 5000-threshold;
            int max=diff*2;
            int count=0;
            for (m=0;m<6;m++)
            {
                if (valTestFace[m]<threshold)
                    count++;
            }
            double min=1000000000;
            for (m=0;m<6;m++)
            {
                if (valTestFace[m]<min)
                    min=valTestFace[m];
            }
            double lowest=5000-min;
            if (lowest<0)
                lowest=0;
            else if (lowest>max)
            {
                lowest=100;
            }
            else
            {
                lowest = (lowest*100)/max;
            }
            (*percentage)=lowest;
            int counttemp=0;
            double valTestFacetemp[6];
            valTestFacetemp[0]=faceSplitReturnVal(image1,testFace);
            valTestFacetemp[1]=faceSplitReturnVal(image2,testFace);
            valTestFacetemp[2]=faceSplitReturnVal(image3,testFace);
            valTestFacetemp[3]=faceSplitReturnVal(image4,testFace);
            valTestFacetemp[4]=faceSplitReturnVal(image5,testFace);
            valTestFacetemp[5]=faceSplitReturnVal(image6,testFace);

            for (m=0;m<6;m++)
            {
                if (valTestFacetemp[m]<threshold2)
                    counttemp++;

            }

            if (count>1 && counttemp>1)  // add
                return 'y';
            cvReleaseImage( &image1);
            cvReleaseImage( &image2);
            cvReleaseImage( &image3);
            cvReleaseImage( &image4);
            cvReleaseImage( &image5);
            cvReleaseImage( &image6);
            cvReleaseImage( &testFace);

        }
        else
            return 'n';

    */
}
int findIndex(char* username)
{
    FILE *fileKey;
    if ( !(fileKey = fopen("/etc/pam-face-authentication/facemanager/face.key", "r+")) )
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
    if ( !(fileKey = fopen("/etc/pam-face-authentication/facemanager/face.key", "r")) )
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
        }
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
    fileStorage = cvOpenFileStorage( "/etc/pam-face-authentication/facedata.xml", 0, CV_STORAGE_READ );
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
    cvReleaseFileStorage( &fileStorage );

    return 1;
}
