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
#include <pwd.h>
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include "pam_face_defines.h"
#include "libfaceauthenticate.h"



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

double LBPdiff(    IplImage* image1,    IplImage* image2)

{
    double weights[7][4]  =
    {
        {.2, .2, .2, .2},
        { 1,  1.5,1.5,1},
        { 1,  2,  2,    1},
        {.5,  1.2, 1.2,.5},
        {.3,  1.2,  1.2, .3},
        {.3,  1.1,  1.1,  .3},
        {.1,  .5,     1, .1 }
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



char recognize(char* username,int currentUserId)
{


    char userFile[300];
    sprintf(userFile, SYSCONFDIR "/pam-face-authentication/%s.jpg",username);
  IplImage * img = cvLoadImage(userFile,0);
   //IplImage * img = cvLoadImage("/home/darksid3hack0r/Desktop/root.jpg",0);
    //  printf("%s \n",userFile);

    char temp[200];
    char featuresConfig[300];

    char* sDistanceThreshold;

    double distanceThreshold1=0;
    double computedDistance1=0;
    double distanceThreshold2=0;
    double computedDistance2=0;
    double distanceThreshold3=0;
    double computedDistance3=0;


    double weights[7][4]  =
    {
        {.2,   1.75,  1.75, .2},
        { 1,  1.5, 1.5,  1},
        { 1,  2  ,   2,  1},
        {.5,  1.2, 1.2, .5},
        {.3,  1.2, 1.2, .3},
        {.3,  1.1, 1.1, .3},
        {.1,  .5,   .5, .1}
    };
    char *buffer;
    unsigned long fileLen;
    struct passwd *userpasswd;
    userpasswd = getpwnam(username);

    sprintf(featuresConfig,"%s/.pam-face-authentication/features/featuresDistance", userpasswd->pw_dir);
    FILE *fileFeaturesDistance,*fileFeaturesAverage;
    if (!(fileFeaturesDistance =fopen(featuresConfig,"r")))
        return 'n';
   // printf("%s \n",featuresConfig);

    fseek(fileFeaturesDistance, 0, SEEK_END);
    fileLen=ftell(fileFeaturesDistance);
    fseek(fileFeaturesDistance, 0, SEEK_SET);
    buffer=(char *)calloc((fileLen+1),sizeof(char));

    fread(buffer,1,fileLen,fileFeaturesDistance);
    sDistanceThreshold=strtok(buffer,"  \n");
    distanceThreshold1=(double)atof(sDistanceThreshold);
    sDistanceThreshold=strtok(NULL,"  \n");
    distanceThreshold2=(double)atof(sDistanceThreshold);
    sDistanceThreshold=strtok(NULL,"  \n");
    distanceThreshold3=(double)atof(sDistanceThreshold);
//printf("%e %e %e\n",distanceThreshold1,distanceThreshold2,distanceThreshold3);

//    distanceThreshold=atof(sDistanceThreshold);
    fclose(fileFeaturesDistance);
free(buffer);
    sprintf(featuresConfig,"%s/.pam-face-authentication/features/featuresAverage", userpasswd->pw_dir);
    if (!(fileFeaturesAverage =fopen(featuresConfig,"r")))
        return 'n';


    fseek(fileFeaturesAverage, 0, SEEK_END);
    fileLen=ftell(fileFeaturesAverage);
    fseek(fileFeaturesAverage, 0, SEEK_SET);
    buffer=(char *)calloc((fileLen+1),sizeof(char));
    float* featuresAverage1 = (float*)calloc(4*7 * 59 ,sizeof(float) );
    float* featuresAverage2 = (float*)calloc(4*7 * 59 ,sizeof(float) );
    float* featuresAverage3 = (float*)calloc(4*7 * 59 ,sizeof(float) );

    if (!buffer)
    {
        fprintf(stderr, "Memory error!");
        fclose(fileFeaturesAverage);
        return 'n';
    }
    fread(buffer,1,fileLen,fileFeaturesAverage);
    char *word1;
    int index=0;
    word1=strtok(buffer,"  \n");
    while (word1!=NULL)
    {
        //printf("%e BUFFER\n",atof(word1));
        if (index<1652)
            featuresAverage1[index]=(float)atof(word1);
        if (index>=1652 && index<3304)
            featuresAverage2[index-1652]=(float)atof(word1);
        if (index>=3304)
            featuresAverage3[index-3304]=(float)atof(word1);
        index++;
        word1=strtok(NULL,"  \n");
    }
    free(buffer);
   fclose(fileFeaturesAverage);

   // printf("%d INDEX\n",index);

    int Nx = floor((img->width - 4)/4);
    int Ny= floor((img->height - 4)/4);
    float* features = (float*)malloc(  Nx*Ny * 18 * sizeof(float) );


    featureDctMod2(img,features);

    int Nx1 = floor((img->width)/30);
    int Ny1= floor((img->height)/20);
    float* featuresLBP = (float*)malloc(  Nx1*Ny1 * 59 * sizeof(float) );
    featureLBPHist(img,featuresLBP);

    int i,j,k=0;
    sprintf(temp,"%d",getuid()); //uid doesnt matter over here
    FILE *f1 =fopen( SYSCONFDIR "/pam-face-authentication/testFeaturesDCT","w");
    FILE *f2 =fopen( SYSCONFDIR "/pam-face-authentication/testFeaturesLBP","w");

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
                double sumHist;
                sumHist=(featuresLBP[i*59*4 + j*59 +k]+featuresAverage1[i*59*4 + j*59 +k])+1;
                computedDistance1+=(weights[i][j]*pow((featuresLBP[i*59*4 + j*59 +k]-featuresAverage1[i*59*4 + j*59 +k]),2))/sumHist;

                sumHist=(featuresLBP[i*59*4 + j*59 +k]+featuresAverage2[i*59*4 + j*59 +k])+1;
                computedDistance2+=(weights[i][j]*pow((featuresLBP[i*59*4 + j*59 +k]-featuresAverage2[i*59*4 + j*59 +k]),2))/sumHist;


                sumHist=(featuresLBP[i*59*4 + j*59 +k]+featuresAverage3[i*59*4 + j*59 +k])+1;
                computedDistance3+=(weights[i][j]*pow((featuresLBP[i*59*4 + j*59 +k]-featuresAverage3[i*59*4 + j*59 +k]),2))/sumHist;

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
//printf("%e %e %e %e %e %e\n",featuresAverage1[0],featuresAverage1[1651],featuresAverage2[0],featuresAverage2[1651],featuresAverage3[0],featuresAverage3[1651]);

    computedDistance1=sqrt(computedDistance1);
    computedDistance2=sqrt(computedDistance2);
    computedDistance3=sqrt(computedDistance3);
   // printf("%e %e %e\n",computedDistance1,computedDistance2,computedDistance3);

//   return 'n';

//   computedDistance=sqrt(computedDistance);

//    printf( " %e %e %e $ %e %e %e \n ",computedDistance1,computedDistance2,computedDistance3,distanceThreshold1,distanceThreshold2,distanceThreshold3);

//   printf("%e computed distance %e threshold of the face from the actual face class\n",computedDistance,distanceThreshold);
    double thresholdEmpericalDistance=51.0;
    if (distanceThreshold1<thresholdEmpericalDistance && distanceThreshold1!=0)
    distanceThreshold1=thresholdEmpericalDistance;

    if (distanceThreshold2<thresholdEmpericalDistance && distanceThreshold2!=0)
    distanceThreshold2=thresholdEmpericalDistance;

    if (distanceThreshold3<thresholdEmpericalDistance && distanceThreshold3!=0)
    distanceThreshold3=thresholdEmpericalDistance;

    int flag=1;

    if (computedDistance1<(distanceThreshold1) && distanceThreshold1!=0)
        flag=-1;

    if (computedDistance2<(distanceThreshold2) && distanceThreshold2!=0)
        flag=-1;
    if (computedDistance3<(distanceThreshold3) && distanceThreshold3!=0)
        flag=-1;

   //     printf( " %e %e %e $ %e %e %e \n ",computedDistance1,computedDistance2,computedDistance3,distanceThreshold1,distanceThreshold2,distanceThreshold3);

free(featuresAverage1);
free(featuresAverage2);
 free(featuresAverage3);
cvReleaseImage( &img);
    if (flag==1)

        return 'n';

    //printf(" \n 1testing");
    char* argv1[4]={"svm-scale","-r", SYSCONFDIR "/pam-face-authentication/featuresDCT.range", SYSCONFDIR "/pam-face-authentication/testFeaturesDCT"};
    FILE *fp5=fopen( SYSCONFDIR "/pam-face-authentication/testFeaturesDCT.scale","w");
//    printf(" \n 2testing");

    svmScale(4,argv1,fp5);

    char* argv2[4]={"svm-scale","-r", SYSCONFDIR "/pam-face-authentication/featuresLBP.range", SYSCONFDIR "/pam-face-authentication/testFeaturesLBP"};
    FILE *fp6=fopen( SYSCONFDIR "/pam-face-authentication/testFeaturesLBP.scale","w");
    svmScale(4,argv2,fp6);
    //printf("testing");

    system(BINDIR "/svm-predict -b 1 " SYSCONFDIR "/pam-face-authentication/testFeaturesDCT.scale " SYSCONFDIR "/pam-face-authentication/featuresDCT.scale.model " SYSCONFDIR "/pam-face-authentication/prediction");
    int ans;
    double sum=0;
    int ansMatch=-1;
    int login=1;
    double percentage1;
    int num=parseSvmPrediction(&ans,&percentage1);
    double cutoff=0;
    cutoff=70 + (double)(20/(num));
    cutoff/=100;
    //printf("%e \n",cutoff);

    ansMatch=ans;
    if (ans!=currentUserId)
        return 'n';
    sum+=percentage1;
//printf("Answer %d \n",ans);
//    printf("DCT Percent %e \n",percentage1);
    if (percentage1<cutoff)
        login=-1;

//printf("Login %d \n",login);
//printf("Answer %d Percentage %e DCT \n",ans,percentage1);
    system(BINDIR "/svm-predict -b 1 " SYSCONFDIR "/pam-face-authentication/testFeaturesLBP.scale " SYSCONFDIR "/pam-face-authentication/featuresLBP.scale.model " SYSCONFDIR "/pam-face-authentication/prediction");
    parseSvmPrediction(&ans,&percentage1);
//printf("Answer %d \n",ans);
//printf("LBP Percent %e \n",percentage1);
    if (ansMatch==ans)
    {
        sum+=percentage1;
    }
//printf("SUM Percent %e \n",sum);
    if (login!=-1)
    {
        if (percentage1<cutoff)
            login=-1;
    }
//printf("Login %d \n",login);
    if (sum<(2*cutoff))
    {
        login=-1;
    }
   // printf("Answer %d Percentage %e LBP \n",ans,percentage1);

//printf("%e  RECOG RATE \n ",sum);

    //printf("Login %d \n",login);

    if (login==-1)
        return 'n';
    else
    {
        // Second Checkpoint

        return 'y';

    }


}
