#include "cv.h"
#include "cvaux.h"
#include <stdio.h>
#include <stdlib.h>

void  featureDctMod2(IplImage * img,float*  features_final)
{

    int Nx = floor((img->width - 4)/4);
    int Ny= floor((img->height - 4)/4);
    float* observations = (float*)malloc(  Nx*Ny * 25 * sizeof(float) );
    float* features = (float*)malloc(  Nx*Ny * 18 * sizeof(float) );

    cvImgToObs_DCT(img,observations, cvSize(8,8),cvSize(5,5),cvSize(4,4) );
    int i,j,k=0;

    for (i=0;i<Ny;i++)
    {
        for (j=0;j<Nx;j++)
        {

/*
0   1   2    3    4
5   6   7    8    9
10  11  12   13  14
15  16  17   18  19
20  21  22   23  24

0,1,5,10,6,2,3,7,11,15,20,16,12,8,4

*/
// Zig Zag 15 features + 3 for dct mod2

            features[i*18*Nx + j*18 +0]=observations[i*25*Nx + j*25 +0]; // not putting zero , might cause scale problem
            features[i*18*Nx + j*18 +1]=observations[i*25*Nx + j*25 +1];
            features[i*18*Nx + j*18 +2]=observations[i*25*Nx + j*25 +5];
            features[i*18*Nx + j*18 +3]=observations[i*25*Nx + j*25 +0];
            features[i*18*Nx + j*18 +4]=observations[i*25*Nx + j*25 +1];
            features[i*18*Nx + j*18 +5]=observations[i*25*Nx + j*25 +5];
            features[i*18*Nx + j*18 +6]=observations[i*25*Nx + j*25 +10];
            features[i*18*Nx + j*18 +7]=observations[i*25*Nx + j*25 +6];
            features[i*18*Nx + j*18 +8]=observations[i*25*Nx + j*25 +2];
            features[i*18*Nx + j*18 +9]=observations[i*25*Nx + j*25 +3];
            features[i*18*Nx + j*18 +10]=observations[i*25*Nx + j*25 +7];
            features[i*18*Nx + j*18 +11]=observations[i*25*Nx + j*25 +11];
            features[i*18*Nx + j*18 +12]=observations[i*25*Nx + j*25 +15];
            features[i*18*Nx + j*18 +13]=observations[i*25*Nx + j*25 +20];
            features[i*18*Nx + j*18 +14]=observations[i*25*Nx + j*25 +16];
            features[i*18*Nx + j*18 +15]=observations[i*25*Nx + j*25 +12];
            features[i*18*Nx + j*18 +16]=observations[i*25*Nx + j*25 +8];
            features[i*18*Nx + j*18 +17]=observations[i*25*Nx + j*25 +4];




            for (k=0;k<18;k++)
            {

                   features_final[i*18*Nx + j*18 +k]= features[i*18*Nx + j*18 +k];

            }
        //    printf("Next Line\n");
        }

    }

    for (i=1;i<Ny-1;i++)
    {
        for (j=1;j<Nx-1;j++)
        {
   //Calculate Horizontal Delta and Vertical Delta
            features_final[i*18*Nx + j*18 ]=(-features[i*18*Nx + (j-1)*18 +3] +features[i*18*Nx + (j+1)*18 +3])/2;
            features_final[i*18*Nx + j*18 +1]=(-features[i*18*Nx + (j-1)*18 +4] +features[i*18*Nx + (j+1)*18 +4])/2;
            features_final[i*18*Nx + j*18 +2]=(-features[i*18*Nx + (j-1)*18 +5] +features[i*18*Nx + (j+1)*18 +5])/2;
            features_final[i*18*Nx + j*18 +3]=(-features[(i-1)*18*Nx + j*18 +3] +features[(i+1)*18*Nx + j*18 +3])/2;
            features_final[i*18*Nx + j*18 +4]=(-features[(i-1)*18*Nx + j*18 +4] +features[(i+1)*18*Nx + j*18 +4])/2;
            features_final[i*18*Nx + j*18 +5]=(-features[(i-1)*18*Nx + j*18 +5] +features[(i+1)*18*Nx + j*18 +5])/2;

        }

    }
free(features);
free(observations);

}/*
void main()
{
IplImage * img = cvLoadImage("abc.jpg",0);
int Nx = floor((img->width - 4)/4);
int Ny= floor((img->height - 4)/4);
float* features = (float*)malloc(  Nx*Ny * 18 * sizeof(float) );
featureDctMod2(img,features);
cvNamedWindow( "Source", 1 );
cvShowImage( "Source", img  );

  cvWaitKey(0);
}
*/
