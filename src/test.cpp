
#include <math.h>
#include <float.h>
#include <limits.h>
#include "cv.h"
#include <cxcore.h>
#include <highgui.h>

#include <stdio.h>
int peakToSideLobeRatio(CvMat*maceFilterVisualize,IplImage *img,int rad1,int rad2);
void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr );
CvMat *computeMace(IplImage **img,int size);
IplImage *  featureLBPSum(IplImage * img);
int checkBit(int i);
double getBIT(IplImage* img,double px,double py,double threshold);
int file_exists(const char* filename);

int main(void)
{
int i=0;
CvMat **filters =new CvMat*[50];

for(i=1;i<8;i++)
{
// is -1 right?
char filename1[300],filename2[300];
sprintf(filename1,"/home/darksid3hack0r/ipaper/database/%d/1.jpg",i);
sprintf(filename2,"/home/darksid3hack0r/ipaper/database/%d/3.jpg",i);
IplImage* image1=cvLoadImage(filename1, -1);
IplImage* image2=cvLoadImage(filename2, -1);
//DO processing here
IplImage **imageNew=new IplImage*[2];
imageNew[0]=image1;
imageNew[1]=image2;
filters[i]=computeMace(imageNew,2);
}
int valsum=0;
//Verify loop
for(i=1;i<8;i++)
{


//do stuff with this val , put in a txt file etc , to analyze it or GNu Plot it
}
//
int j;
int valsum1=0;
int max=0;
int maxl=0;
int maxm=0;
//IMPOSTER VALUES
int l,m=0;
for(l=14;l<50;l+=2)
{
for(m=l;m<50;m+=2)
{
 valsum1=0;
valsum=0;

for(i=1;i<8;i++)
{

for(j=1;j<8;j++)
{
if(i!=j)
{
char filename1[300],filename2[300];
sprintf(filename1,"/home/darksid3hack0r/ipaper/database/%d/4.jpg",j);
IplImage* image1=cvLoadImage(filename1,1);
int val=peakToSideLobeRatio(filters[i],image1,m,l);
cvReleaseImage(&image1);
//printf("%d\n",val);
valsum1+=val;
}
else
{
char filename1[300],filename2[300];
sprintf(filename1,"/home/darksid3hack0r/ipaper/database/%d/2.jpg",i);
IplImage* image1=cvLoadImage(filename1,1);
int val=peakToSideLobeRatio(filters[i],image1,m,l);
valsum+=val;
cvReleaseImage(&image1);
}
}

//do stuff with this val , put in a txt file etc , to analyze it or GNu Plot it
}
if(max<((valsum/7)-((valsum1)/(6*7))))
{max=((valsum/7)-((valsum1)/(6*7)));
maxl=l;
maxm=m;
}
printf("%d %d %d \n ",l,m,((valsum/7)-((valsum1)/(6*7))));

valsum1=0;
valsum=0;

}}


printf("MAX LM%d %d %d\n",max,maxl,maxm);


}
