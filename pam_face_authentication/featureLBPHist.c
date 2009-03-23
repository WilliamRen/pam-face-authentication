#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <math.h>

int checkBit(int i)
{
// Well Removing patterns with bits changing consecutively  more than twice
    int j=i;
    int bit8=(i%2);
    int bit7=((i/2)%2);
    int bit6=((i/4)%2);
    int bit5=((i/8)%2);
    int bit4=((i/16)%2);
    int bit3=((i/32)%2);
    int bit2=((i/64)%2);
    int bit1=((i/128)%2);
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
void  featureLBPHist(IplImage * img,float *features_final)
{
    int lbpArry[256];

    IplImage* imgLBP=cvCreateImage( cvSize(img->width,img->height), 8, img->nChannels );
    int Nx = floor((img->width )/30);
    int Ny= floor((img->height)/20);

    int i,j=0;

    cvZero(imgLBP);
    for (i=0;i<img->height;i++)
    {

        for (j=0;j<img->width;j++)
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
            s=cvGet2D(img,i,j);
            double bit1=128*getBIT(img,p1x,p1y,s.val[0]);
            double bit2=64*getBIT(img,p2x,p2y,s.val[0]);
            double bit3=32*getBIT(img,p3x,p3y,s.val[0]);
            double bit4=16*getBIT(img,p4x,p4y,s.val[0]);
            double bit5=8*getBIT(img,p5x,p5y,s.val[0]);
            double bit6=4*getBIT(img,p6x,p6y,s.val[0]);
            double bit7=2*getBIT(img,p7x,p7y,s.val[0]);
            double bit8=1*getBIT(img,p8x,p8y,s.val[0]);
            CvScalar s1;
            s1.val[0]=bit1+bit2+bit3+bit4+bit5+bit6+bit7+bit8;
            s1.val[1]=0;
            s1.val[2]=0;
            cvSet2D(imgLBP,i,j,s1);
        }
    }

    int l,m,k=0;
    for (i=0;i<Ny;i++)
    {
        for (j=0;j<Nx;j++)
        {
            int startX=30*j;
            int startY=20*i;
            int count=0;
            for (k=0;k<256;k++)
            {
                if(checkBit(k)==0)
                {
                lbpArry[k]=count;


                  features_final[i*Nx*59 + j*59 +count]=0;
                        count++;
                }
                else
                {
                lbpArry[k]=58;
                 features_final[i*Nx*59 + j*59 +58]=0;
                 }
            }
//printf("%d \n",count);

            for (l=0;l<20;l++)
            {

                for (m=0;m<30;m++)
                {
                    CvScalar s;
                    s=cvGet2D(imgLBP,startY+l,startX+m);
                    int val=s.val[0];
                    features_final[i*Nx*59 + j*59 +lbpArry[val]]++;
                }
            }

        }
    }

cvReleaseImage( &imgLBP);
}
/*
void createLBP(char * fullpath1,char * fullpath2)
{
    printf("%s %s \n",fullpath1,fullpath2);

    IplImage* img=cvLoadImage(fullpath1,0);
   IplImage* imgLBP=cvCreateImage( cvSize(img->width,img->height), 8, img->nChannels );
    int Nx = floor((img->width - 10)/10);
    int Ny= floor((img->height - 10)/10);

    int i,j=0;

    cvZero(imgLBP);
    for (i=0;i<img->height;i++)
    {

        for (j=0;j<img->width;j++)
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
            s=cvGet2D(img,i,j);
            double bit1=128*getBIT(img,p1x,p1y,s.val[0]);
            double bit2=64*getBIT(img,p2x,p2y,s.val[0]);
            double bit3=32*getBIT(img,p3x,p3y,s.val[0]);
            double bit4=16*getBIT(img,p4x,p4y,s.val[0]);
            double bit5=8*getBIT(img,p5x,p5y,s.val[0]);
            double bit6=4*getBIT(img,p6x,p6y,s.val[0]);
            double bit7=2*getBIT(img,p7x,p7y,s.val[0]);
            double bit8=1*getBIT(img,p8x,p8y,s.val[0]);
            CvScalar s1;
            s1.val[0]=bit1+bit2+bit3+bit4+bit5+bit6+bit7+bit8;
            s1.val[1]=0;
            s1.val[2]=0;
            cvSet2D(imgLBP,i,j,s1);
        }
    }

    cvSaveImage(fullpath2,imgLBP);
    cvReleaseImage( &img);
    cvReleaseImage( &imgLBP);

}

/*
void main()
{

IplImage * img = cvLoadImage("abc.jpg",0);
int Nx = floor((img->width - 10)/10);
int Ny= floor((img->height - 10)/10);
float* features = (float*)malloc(  Nx*Ny * 59 * sizeof(float) );

featureLBPHist(img,features);
//cvNamedWindow( "Source", 1 );
//cvShowImage( "Source", img  );
  cvWaitKey(0);

}

*/
