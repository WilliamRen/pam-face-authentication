#include "eyesDetector.h"
#include "pam_face_defines.h"
#include <stdio.h>
#include <highgui.h>
#include <cv.h>

double CenterofMass(IplImage* src,int flagXY);
double difference(double * currentIntegralImage,double * integralImage,int size2, double px,double py);

char *HAAR_CASCADE_EYE=PKGDATADIR "/haarcascade_eye_tree_eyeglasses.xml";
char *HAAR_CASCADE_EYE_2=PKGDATADIR "/haarcascade_eye.xml";
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


IplImage* featureLBPSum(IplImage * img)
{
    double sum=0;
    IplImage* imgLBP=cvCreateImage( cvSize(img->width,img->height), 8, img->nChannels );
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

return imgLBP;
}


double eyesDetector::gridSearch(double  scaleFactor,double translateFactor,double * updatedScaleFactor,double * updatedTranslateFactor,double *currentIntegralImage,double* integralModel,int size,double scale,double translate)
{
    //printf("gridSearch Int %e \n",integralImage[75]);
    double val,min=0;
    min=difference(currentIntegralImage,integralModel,size,scaleFactor,translateFactor);
    //printf("%e MIN \n",min);
    val=min;
    double px,py;
    px=scaleFactor;
    py=translateFactor;
    *(updatedScaleFactor)=px;
    *(updatedTranslateFactor)=py;
    int i,j=0;
    for (i=0;i<9;i++)
    {
        for (j=0;j<9;j++)
        {
            px=scaleFactor - i*scale;
            py=translateFactor + j*translate;
            val=difference(currentIntegralImage,integralModel,size,px,py);
            if (min>val)
            {

                min=val;
                *(updatedScaleFactor)=px;
                *(updatedTranslateFactor)=py;
               printf("%e %e FX FY \n",*updatedScaleFactor,*updatedTranslateFactor);

            }
        }
    }

    return min;
}

double difference(double * currentIntegralImage,double * integralImage,int size2, double px,double py)
{
    //printf("Difference Int %e \n",integralImage[75]);

    double diff=0;
    int i=0;
    for (i=0;i<size2;i++)
    {
        if ((px*i +py)>=0 && (px*i +py) <size2)
        {
            int k=floor(px*i + py);
            double decimal =px*i + py - k;
            double val  = currentIntegralImage[k] + decimal*(currentIntegralImage[k+1]- currentIntegralImage[k]);
            //printf("%e %e %d \n",val,integralImage[i],i);
            diff+=pow(integralImage[i] -val,2);
            //printf("%e diff value \n",diff);

        }
    }
    return diff;

}

eyesDetector::eyesDetector()
{

    nested_cascade = (CvHaarClassifierCascade*)cvLoad(HAAR_CASCADE_EYE, 0, 0, 0 );
    nested_cascade_2    = (CvHaarClassifierCascade*)cvLoad(HAAR_CASCADE_EYE_2, 0, 0, 0 );
    storage = cvCreateMemStorage(0);
    cvClearMemStorage( storage );
    eyesInformation.LE =cvPoint(0,0);
    eyesInformation.RE =cvPoint(0,0);
    eyesInformation.Length =0;
    integralImageYLeft=0;
    integralImageXLeft=0;
    integralImageYRight=0;
    integralImageXRight=0;

    integralEyeLeft=cvPoint(0,0);
    integralEyeRight=cvPoint(0,0);
    integralImageSizeY=0;
    integralImageSizeX=0;


}
int eyesDetector::checkEyeDetected()
{
    if (eyesInformation.Length!=0)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

double eyesDetector::runGridSearch(IplImage *gray,int size,int flag,double *d,double* e ,double * integralTemp)
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
    double *currentIntegralImage=0;

    currentIntegralImage=calculateIntegral(grayNew,flag);

    double slLimitUp =1.1;
    double tlLimidUp=-10;
    double slLimitLw =.9;
    double tlLimidLw=10;
    double num=8;

    double  scaleFactor=slLimitUp;
    double  translateFactor=tlLimidUp;
    double  updatedScaleFactor=scaleFactor,updatedTranslateFactor=tlLimidUp;
    double scale=(slLimitUp-slLimitLw)/num;
    double translate=(tlLimidLw-tlLimidUp)/num;
    int l=0;
    for (l=0;l<10;l++)
    {

        if ((scaleFactor==slLimitUp) && (translateFactor==tlLimidUp))
        {
            //printf("true \n");

        }


        v =  gridSearch(scaleFactor,translateFactor,&updatedScaleFactor,&updatedTranslateFactor,currentIntegralImage,integralTemp,size,scale,translate);

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

printf("%e %e SCALE  \n",scaleFactor,translateFactor);
    *d=scaleFactor;
    *e=translateFactor;
    return v;
}

void eyesDetector::runEyesDetector(IplImage * input,IplImage * fullImage,CvPoint LT)

{

    static int countR =0;
    printf("%d COUNT \n ",countR);

    eyesInformation.LE =cvPoint(0,0);
    eyesInformation.RE =cvPoint(0,0);
    eyesInformation.Length =0;
    if (input==0)
        return;

    IplImage *gray = cvCreateImage( cvSize(input->width,input->height/2), 8, 1 );
    IplImage *gray_fullimage = cvCreateImage( cvSize(fullImage->width,fullImage->height), 8, 1 );
    cvSetImageROI(input,cvRect(0,(input->height)/8,input->width,(input->height)/2));
    cvCvtColor( input, gray, CV_BGR2GRAY );
    cvResetImageROI(input);


    cvCvtColor( fullImage, gray_fullimage, CV_BGR2GRAY );


    double scale=1,i=0,j=0;
    //  //printf("%e SCALE \n\n",scale);
    cvClearMemStorage( storage );
    CvSeq* nested_objects = cvHaarDetectObjects(gray, nested_cascade, storage,1.4, 2, 0,cvSize(0,0) );
    int count=nested_objects ? nested_objects->total : 0;
    if (count==0)
    {
        nested_objects = cvHaarDetectObjects( gray, nested_cascade_2, storage,1.4, 2, 0,cvSize(0,0) );
    }
    int leftT=0,rightT=0;
    count=nested_objects ? nested_objects->total : 0;
    int Flag=0;
    if (count<2 )
    {
        Flag=1;
    }
    else
    {

        for ( j = 0; j < (nested_objects ? nested_objects->total : 0); j++ )
        {
            CvPoint center;
            CvRect* nr = (CvRect*)cvGetSeqElem( nested_objects, j );
            center.x = cvRound((LT.x+ nr->x + nr->width*0.5)*scale);
            center.y = cvRound((LT.y + (input->height)/8+nr->y + nr->height*0.5)*scale);

            if ((center.x-4)>0 && (center.x-4)<(IMAGE_WIDTH-8) && (center.y-4)>0  && (center.y-4)<(IMAGE_HEIGHT-8))
            {
                cvSetImageROI(gray_fullimage,cvRect(center.x-4,center.y-4,8,8));
                IplImage* eyeDetect = cvCreateImage(cvSize(8,8),8,1);
                cvResize( gray_fullimage,eyeDetect, CV_INTER_LINEAR ) ;
                cvResetImageROI(gray_fullimage);

                double xCordinate=(center.x-4+CenterofMass(eyeDetect,0))*scale;
                double yCordinate=(center.y-4+CenterofMass(eyeDetect,1))*scale;

                cvReleaseImage( &eyeDetect );
                if (center.x<cvRound((LT.x + input->width*0.5)*scale))
                {
                    eyesInformation.LE.x=xCordinate;
                    eyesInformation.LE.y=yCordinate;
                    //cvCircle( fullImage, cvPoint(eyesInformation.LE.x,eyesInformation.LE.y), 4, CV_RGB(128,128,128), 1, 8, 0 );
                    leftT=1;
                }
                else
                {

                    eyesInformation.RE.x=xCordinate;
                    eyesInformation.RE.y=yCordinate;
                    //cvCircle( fullImage, cvPoint(eyesInformation.RE.x,eyesInformation.RE.y), 4, CV_RGB(128,128,128), 1, 8, 0 );

                    rightT=1;
                }

            }
        }



        if (leftT==1 && rightT==1)
        {
            countR++;

            integralEyeLeft.x=eyesInformation.LE.x-LT.x;
            integralEyeLeft.y=eyesInformation.LE.y-LT.y-(input->height)/8;
            integralEyeRight.x=eyesInformation.RE.x-LT.x -((gray->width)/2);
            integralEyeRight.y=eyesInformation.RE.y-LT.y-(input->height)/8;
            if (integralImageXLeft!=0)
                delete [] integralImageXLeft;
            if (integralImageYLeft!=0)
                delete [] integralImageYLeft;

            if (integralImageXRight!=0)
                delete [] integralImageXRight;
            if (integralImageYRight!=0)
                delete [] integralImageYRight;


            integralImageSizeY=input->height/2;
            integralImageSizeX=input->width/2;

            IplImage* grayIm1 = cvCreateImage(cvSize(gray->width/2,gray->height),8,1);

            cvSetImageROI(gray,cvRect(0,0,(gray->width)/2,(gray->height)));
            cvResize(gray, grayIm1, CV_INTER_LINEAR ) ;
            cvResetImageROI(gray);
            IplImage* grayIm2 = cvCreateImage(cvSize(gray->width/2,gray->height),8,1);

            cvSetImageROI(gray,cvRect(gray->width/2,0,gray->width/2,gray->height));
            cvResize(gray,grayIm2, CV_INTER_LINEAR ) ;
            cvResetImageROI(gray);


            integralImageYLeft= calculateIntegral(grayIm1,1);
            integralImageXLeft= calculateIntegral(grayIm1,0);

            integralImageYRight= calculateIntegral(grayIm2,1);
            integralImageXRight= calculateIntegral(grayIm2,0);
            cvReleaseImage(&grayIm1);
            cvReleaseImage(&grayIm2);

            eyesInformation.Length=sqrt(pow(eyesInformation.RE.y-eyesInformation.LE.y,2)+ pow(eyesInformation.RE.x-eyesInformation.LE.x,2));
        }

    }



    CvPoint p1,p2;
    p1.x=0;
    p1.y=0;
    p2.x=0;
    p2.y=0;

    if (integralImageSizeX!=0 && integralImageSizeY!=0)
    {
        double d=0,e=0;
        double v1,v2;
        IplImage* grayIm1 = cvCreateImage(cvSize(gray->width/2,gray->height),8,1);
        cvRectangle( fullImage,cvPoint(LT.x,(input->height)/8+LT.y), cvPoint(LT.x + gray->width/2, LT.y+ (input->height)/8+gray->height), CV_RGB(255,0,0), 3, 8, 0 );

        cvSetImageROI(gray,cvRect(0,0,gray->width/2,gray->height));
        cvResize( gray,grayIm1, CV_INTER_LINEAR ) ;
        cvResetImageROI(gray);
        IplImage* grayIm2 = cvCreateImage(cvSize(gray->width/2,gray->height),8,1);
        cvRectangle( fullImage,cvPoint(LT.x + gray->width/2,(input->height)/8+LT.y), cvPoint(LT.x + gray->width,(input->height)/8+ LT.y+ gray->height), CV_RGB(255,0,0), 3, 8, 0 );

        cvSetImageROI(gray,cvRect(gray->width/2,0,gray->width/2,gray->height));
        cvResize(gray,  grayIm2,CV_INTER_LINEAR ) ;
        cvResetImageROI(gray);

        v1=runGridSearch(grayIm1,integralImageSizeY,1,&d,&e,integralImageYLeft);
        p1.y=LT.y+(input->height)/8+floor((double)(d*(double)(integralEyeLeft.y) + (double)(e))*((double)(input->height)/(double)(2*(double)(integralImageSizeY))));
        v2=runGridSearch(grayIm1,integralImageSizeX,0,&d,&e,integralImageXLeft);
        p1.x=LT.x+floor((double)(d*(double)integralEyeLeft.x + (double)(e))*((double)(input->width)/(double)(2*integralImageSizeX)));



        v1=runGridSearch(grayIm2,integralImageSizeY,1,&d,&e,integralImageYRight);
        p2.y=LT.y+(input->height)/8+floor((double)(d*(double)(integralEyeRight.y) + (double)(e))*((double)(input->height)/(2*(double)(integralImageSizeY))));
        v2=runGridSearch(grayIm2,integralImageSizeX,0,&d,&e,integralImageXRight);
        p2.x=LT.x+(gray->width/2)+floor((double)(d*(double)integralEyeRight.x + (double)(e))*((double)(input->width)/(double)(2*integralImageSizeX)));

        eyesInformation.LE.x=p1.x;
        eyesInformation.LE.y=p1.y;
        eyesInformation.RE.x=p2.x;
        eyesInformation.RE.y=p2.y;
        eyesInformation.Length=0;
        eyesInformation.Length=sqrt(pow(eyesInformation.RE.y-eyesInformation.LE.y,2)+ pow(eyesInformation.RE.x-eyesInformation.LE.x,2));


        cvReleaseImage(&grayIm1);
        cvReleaseImage(&grayIm2);


    }








    cvReleaseImage(&gray);


}
