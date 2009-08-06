/** @file */
/*
    INTEGRAL AND VARIANCE PROJECTION TRACKER CLASS
    Copyright (C) 2009 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus
    http://code.google.com/p/pam-face-authentication/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define NUMBER_OF_GRID_POINTS 25
#define GRID_SIDE_SIZE 5
#include "cv.h"
#include <stdio.h>
#include "tracker.h"
tracker::tracker(void)
{
    trackerModelFeatureSizeY=0;
    trackerModelFeatureSizeX=0;
    trackerModelFeatureVARIANCEY= 0;
    trackerModelFeatureVARIANCEX= 0;
    trackerModelFeatureINTEGRALX= 0;
    trackerModelFeatureINTEGRALY= 0;
    stateVariableScaleX=0;
    stateVariableTranslateX=0;
    stateVariableScaleY=0;
    stateVariableTranslateY=0;
    lastImageWidth=0;
    lastImageHeight=0;
}
void tracker::trackImage(IplImage * input)
{
    lastImageHeight=input->height;
    lastImageWidth=input->width;

    double v1=runGridSearch(input,trackerModelFeatureSizeY,1,&stateVariableScaleY,&stateVariableTranslateY,trackerModelFeatureVARIANCEY,trackerModelFeatureINTEGRALY,anchorPoint.y);
    double  v2=runGridSearch(input,trackerModelFeatureSizeX,0,&stateVariableScaleX,&stateVariableTranslateX,trackerModelFeatureVARIANCEX,trackerModelFeatureINTEGRALX,anchorPoint.x);


    lastDifference1=v1;
    lastDifference2=v2;
//if(v1>10000 && v2>10000)
 //   printf("%e %e V1 V2 \n",lastDifference1,lastDifference2);

}

void tracker::findPoint(CvPoint p1,CvPoint* p2)
{
    (*p2).x=floor((double)(stateVariableScaleX*(double)p1.x + (double)(stateVariableTranslateX))*((double)(lastImageWidth)/(double)(trackerModelFeatureSizeX)));
    (*p2).y=floor((double)(stateVariableScaleY*(double)p1.y + (double)(stateVariableTranslateY))*((double)(lastImageHeight)/(double)(trackerModelFeatureSizeY)));
}


void tracker:: setModel(IplImage * input)
{
    if (input==0)
        return;
    if (trackerModelFeatureVARIANCEY!=0)
        delete [] trackerModelFeatureVARIANCEY;
    if (trackerModelFeatureVARIANCEX!=0)
      delete [] trackerModelFeatureVARIANCEX;
    if (trackerModelFeatureINTEGRALX!=0)
        delete [] trackerModelFeatureINTEGRALX;
    if (trackerModelFeatureINTEGRALY!=0)
      delete [] trackerModelFeatureINTEGRALY;
    trackerModelFeatureSizeY=input->height;
    trackerModelFeatureSizeX=input->width;
//Variance
    trackerModelFeatureVARIANCEY= calculateFeature(input,1,0);
    trackerModelFeatureVARIANCEX= calculateFeature(input,0,0);       ;
//Integral
    trackerModelFeatureINTEGRALY= calculateFeature(input,1,1);
    trackerModelFeatureINTEGRALX= calculateFeature(input,0,1);
}
double * tracker::calculateFeature(IplImage * input,int flag,int varorintegral)
{

  //cvEqualizeHist( input,input);
    //  IplImage * input =cvCreateImage( cvSize(in->width,in->height), 8, 1 );
    //cvThreshold(in,input,100,255,CV_THRESH_TRUNC);

    double * integral=0;

    int i=0,j=0;

    int lim1,lim2;
    if (flag==0)
    {
        integral=new double[input->width];
        lim1=input->width;
        lim2=input->height;
    }
    else
    {
        integral=new double[input->height];
        lim2=input->width;
        lim1=input->height;
    }
    for (i=0;i<lim1;i++)
        integral[i]=0;
    for (i=0;i<lim1;i++)
    {
        for (j=0;j<lim2;j++)
        {
            CvScalar s;
            if (flag==0)
                s=cvGet2D(input,j,i);
            else
                s=cvGet2D(input,i,j);

            integral[i]+=(s.val[0]);



        }
        integral[i]/=lim2;
    }

    double *var=0;
    if (flag==0)
        var=new double[input->width];
    else
        var=new double[input->height];

    for (i=0;i<lim1;i++)
    {
        var[i]=0;
        for (j=0;j<lim2;j++)
        {
            CvScalar s;

            if (flag==0)
                s=cvGet2D(input,j,i);
            else
                s=cvGet2D(input,i,j);
            var[i]+=pow(((s.val[0])-integral[i]),2);




        }
        var[i]=sqrt(var[i]/lim2);
        //var[i]+=integral[i];
        /*if(i>0)
        {
            if(pow((var[i]-var[i-1]),2)<1)
              var[i]=floor(var[i]);
        }
        printf("%e %e \n",integral[i],var[i]);
        */
    }
    double intSum=0,varSum=0;
     for (i=0;i<lim1;i++)
    {
        intSum+=integral[i];
        varSum+=var[i];
    }
    intSum=intSum/lim1;
    varSum=varSum/lim1;
  for (i=0;i<lim1;i++)
    {
        integral[i]-=intSum;
        var[i]-=varSum;

    }


//   delete [] integral;
    if (varorintegral==1)
        return integral;
    else
        return var;
}

double tracker::findParam(double  scaleFactor,double translateFactor,double * updatedScaleFactor,double * updatedTranslateFactor,double *feature,double *feature1,double* featureModel,double* featureModel1,int size,double scale,double translate,int anchor)
{

    double val;
    double diffVal1[NUMBER_OF_GRID_POINTS];
    double diffVal2[NUMBER_OF_GRID_POINTS];
    int diffValRanks1[NUMBER_OF_GRID_POINTS];
    int diffValRanks2[NUMBER_OF_GRID_POINTS];
    int diffValRanksRev1[NUMBER_OF_GRID_POINTS];
    int diffValRanksRev2[NUMBER_OF_GRID_POINTS];
    double px,py;
    px=scaleFactor;
    py=translateFactor;
    *(updatedScaleFactor)=px;
    *(updatedTranslateFactor)=py;
    int i,j=0;
    for (i=0;i<GRID_SIDE_SIZE;i++)
    {
        for (j=0;j<GRID_SIDE_SIZE;j++)
        {
            px=scaleFactor - i*scale;
            py=translateFactor + j*translate;
            diffVal1[GRID_SIDE_SIZE*i+j]=difference(feature,featureModel,size,px,py,anchor);
            diffVal2[GRID_SIDE_SIZE*i+j]=difference(feature1,featureModel1,size,px,py,anchor);

        }
    }
    for (i=0;i<NUMBER_OF_GRID_POINTS;i++)
    {
        diffValRanks1[i]=i;
        diffValRanks2[i]=i;
    }
    for (i=0;i<NUMBER_OF_GRID_POINTS;i++)
    {
        for (j=0;j<(NUMBER_OF_GRID_POINTS-1);j++)
        {
            if (diffVal1[diffValRanks1[j]]>diffVal1[diffValRanks1[j+1]])
            {
                int temp=diffValRanks1[j];
                diffValRanks1[j]=diffValRanks1[j+1];
                diffValRanks1[j+1]=temp;
            }
            if (diffVal2[diffValRanks2[j]]>diffVal2[diffValRanks2[j+1]])
            {
                int temp=diffValRanks2[j];
                diffValRanks2[j]=diffValRanks2[j+1];
                diffValRanks2[j+1]=temp;
            }
        }
    }


   for (i=0;i<NUMBER_OF_GRID_POINTS;i++)
    {
   for (j=0;j<NUMBER_OF_GRID_POINTS;j++)
    {
    if(diffValRanks1[j]==i)
    diffValRanksRev1[i]=j;
    if(diffValRanks2[j]==i)
    diffValRanksRev2[i]=j;

    }
    }
int sumRank[NUMBER_OF_GRID_POINTS];
     for (i=0;i<NUMBER_OF_GRID_POINTS;i++)
    {
        sumRank[i]=(diffValRanksRev2[i])+ (diffValRanksRev1[i]);
    }

int min=NUMBER_OF_GRID_POINTS+10;
int ind=-1;
        for (i=0;i<NUMBER_OF_GRID_POINTS;i++)
    {

            if(min>sumRank[i])
        {    min=sumRank[i];
            ind=i;
        }

    }

    /*sumRank[ind]=NUMBER_OF_GRID_POINTS+10;
int nextIndex,newMin;
    newMin=NUMBER_OF_GRID_POINTS+10;

        for (i=0;i<NUMBER_OF_GRID_POINTS;i++)
    {

            if(newMin>sumRank[i])
        {    newMin=sumRank[i];
            nextIndex=i;
        }

    }
    */
//if(newMin==min)
//printf("AAAAAAAAAAAAAAAAAAAA\nAAAAAAAAAAAAAAAAAAAA\nAAAAAAAAAAAAAAAAAAAA\nAAAAAAAAAAAAAAAAAAAA\nAAAAAAAAAAAAAAAAAAAA\nAAAAAAAAAAAAAAAAAAAA\nAAAAAAAAAAAAAAAAAAAA\nAAAAAAAAAAAAAAAAAAAA\n");

i=ind/GRID_SIDE_SIZE;
j=(ind) - (i*GRID_SIDE_SIZE);
//printf("%d %d aaaaaaaaaaaa \n",i,j);
            px=scaleFactor - i*scale;
            py=translateFactor + j*translate;
    *(updatedScaleFactor)=px;
    *(updatedTranslateFactor)=py;
double sd= diffVal1[GRID_SIDE_SIZE*i+j] + diffVal2[GRID_SIDE_SIZE*i+j];
    return sd;
}


double tracker::runGridSearch(IplImage *gray,int size,int flag,double *d,double* e ,double * varianceImage,double * integralImage,int anchor)
{

    double v=0;
    double dimension=0;
    if (flag==1)
        dimension=(double)(gray->width)*(double)((double)size/(double)gray->height);
    else
        dimension=(double)(gray->height)*(double)((double)size/(double)gray->width);


    int dimensionFloor=floor(dimension);
    IplImage *grayNew=0;
    if (flag==1)
        grayNew = cvCreateImage( cvSize(dimensionFloor,size), 8, 1 );
    else
        grayNew = cvCreateImage( cvSize(size,dimensionFloor), 8, 1 );

    cvResize( gray, grayNew, CV_INTER_LINEAR );
    double *feature=0;
    double *feature1=0;

    feature=calculateFeature(grayNew,flag,0);
    feature1=calculateFeature(grayNew,flag,1);

    double slLimitUp =1.1;
    double tlLimidUp=-4;
    double slLimitLw =.9;
    double tlLimidLw=4;
    double num=(GRID_SIDE_SIZE-1);

    double  scaleFactor=slLimitUp;
    double  translateFactor=tlLimidUp;
    double  updatedScaleFactor=scaleFactor,updatedTranslateFactor=tlLimidUp;
    double scale=(slLimitUp-slLimitLw)/num;
    double translate=(tlLimidLw-tlLimidUp)/num;
    int l=0;
    for (l=0;l<6;l++)
    {
        v =  findParam(scaleFactor,translateFactor,&updatedScaleFactor,&updatedTranslateFactor,feature,feature1,varianceImage,integralImage,size,scale,translate,anchor);

        scale/=2;
        translate/=2;
        scaleFactor=updatedScaleFactor+(num/2)*scale;
        translateFactor=updatedTranslateFactor- (num/2)*translate;

        if (translateFactor<=tlLimidUp)
            translateFactor=tlLimidUp;

        if (scaleFactor>=slLimitUp)
            scaleFactor=slLimitUp;



        if ((scaleFactor-num*scale)<=slLimitLw)
            scaleFactor=slLimitLw+(num*scale);



        if ((translateFactor+num*translate)>=tlLimidLw)
            translateFactor=tlLimidLw-(num*translate);


    }
    *d=scaleFactor;
    *e=translateFactor;
    cvReleaseImage(&grayNew);
    delete [] feature;
    return v;
}

double tracker::difference(double * feature,double * featureModel,int size, double px,double py,int anchor)
{
    double diff=0;
    int i=0;
    for (i=0;i<size;i++)
    {
        if ((px*i +py)>=0 && (px*i +py) <size)
        {
            int k=floor(px*i + py);
            double decimal =px*i + py - k;
            double val  = feature[k] + decimal*(feature[k+1]- feature[k]);





             if( (i>int(anchor - floor(.07*size))) && (i<int(anchor+ floor(.07*size))))
              {
              //    printf("true anchor \n",k);
                diff+= (1.3*pow(featureModel[i] -val,2));


              }
              else
              diff+= (.7*pow(featureModel[i] -val,2));

        }
    }
    return diff;
}
