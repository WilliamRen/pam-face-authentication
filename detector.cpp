/*
    Detector Class - Inherits FACE DETECTOR AND EYE DETECTOR CLASS
    Copyright (C) 2009 Rohan Anil (rohan.anil@gmail.com) -BITS Pilani Goa Campus

    http://code.google.com/p/pam-face-authentication/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "detector.h"
#include "cv.h"
#include <stdio.h>
#include <tracker.h>

#define eyeSidePad 30
#define eyeTopPad 30
#define eyeBottomPad 120

double CenterofMass(IplImage* src,int flagXY);

void rotate(double angle, float centreX, float centreY,IplImage * img,IplImage * dstimg);
void rotatePoint(CvPoint* srcP,CvPoint* dstP,double angle,float centreX, float centreY);

IplImage * preprocess(IplImage * img,CvPoint plefteye,CvPoint prighteye)
{

    IplImage * face= cvCreateImage( cvSize(140,150),8,3);
    IplImage* imgDest = cvCreateImage( cvSize(img->width,img->height),8,3);
    cvZero(face);




    double yvalue=prighteye.y-plefteye.y;
    double xvalue=prighteye.x-plefteye.x;
    double ang= atan(yvalue/xvalue)*(180/CV_PI);
    double width= sqrt(pow(xvalue,2)+pow(yvalue,2));
    double ratio=sqrt(pow(xvalue,2)+pow(yvalue,2))/80;
    double sidePad=eyeSidePad*ratio;
    double topPad=eyeTopPad*ratio;
    double bottomPad=eyeBottomPad*ratio;
    CvPoint p1LeftTop,p1RightBottom;
    p1LeftTop.x=plefteye.x-sidePad;
    p1LeftTop.y=(plefteye.y)-topPad;
    p1RightBottom.x=plefteye.x+width+sidePad;
    p1RightBottom.y=(plefteye.y)  +bottomPad;
    rotate(ang,plefteye.x ,plefteye.y,img,imgDest);
    cvSetImageROI(imgDest,cvRect(p1LeftTop.x,p1LeftTop.y,p1RightBottom.x-p1LeftTop.x,p1RightBottom.y-p1LeftTop.y));
    cvResize(imgDest,face,CV_INTER_LINEAR);
    cvResetImageROI(imgDest);
    return face;
}


detector::detector()
{
    messageIndex=-1;
    paintInformation.LT=cvPoint(0,0);
    paintInformation.RB=cvPoint(0,0);
    paintInformation.LB=cvPoint(0,0);
    paintInformation.RT=cvPoint(0,0);

    paintInformation.LE=cvPoint(0,0);
    paintInformation.RE=cvPoint(0,0);
    paintInformation.Length=0;
    paintInformation.Width=0;
    paintInformation.Height=0;
    clippedFace=0;
    boolClipFace=0;
    totalFaceClipNum=0;
    clipFaceCounter=0;
}

IplImage * detector::clipFace(IplImage * inputImage)
{
    static CvPoint leftEye,rightEye;
    if (inputImage==0)
        return 0;
    if (checkFaceDetected()==1)
    {
        IplImage *face=preprocess(inputImage,eyesInformation.LE,eyesInformation.RE);
        return face;
    }
    else
        return 0;

}


int detector::runDetector(IplImage * input)
{
    messageIndex=-1;
    static int flag;
    if (input==0)
        return -1;
    static tracker leftEye;
    static tracker rightEye;
    static tracker fullTracker;
    static CvPoint leftEyeP,rightEyeP;
    static CvPoint leftEyeP1,rightEyeP1;
    static CvPoint leftEyeP2,rightEyeP2;
    static double inAngle;


    static int lengthEye,widthEyeWindow,heightEyeWindow;

    static int prevlengthEye;

    runFaceDetector(input);

    if (checkFaceDetected()==1)
    {
        if (faceInformation.Width<120 || faceInformation.Height<120)
        {
            messageIndex=0;

        }
        else if (faceInformation.Width>200 || faceInformation.Height>200)
        {

            messageIndex=1;
        }
        else
        {
            IplImage * clipFaceImage=clipDetectedFace(input);
            runEyesDetector(clipFaceImage,input,faceInformation.LT);
            if (checkEyeDetected()==1)
            {
                flag=1;
                IplImage *gray = cvCreateImage( cvSize(clipFaceImage->width,clipFaceImage->height/2), 8, 1 );
                cvSetImageROI(clipFaceImage,cvRect(0,(clipFaceImage->height)/8,clipFaceImage->width,(clipFaceImage->height)/2));
                cvCvtColor( clipFaceImage, gray, CV_BGR2GRAY );
                cvResetImageROI(clipFaceImage);
                leftEyeP.x=eyesInformation.LE.x;
                leftEyeP.y=eyesInformation.LE.y;
                rightEyeP.x=eyesInformation.RE.x;
                rightEyeP.y=eyesInformation.RE.y;
                leftEyeP2.x=eyesInformation.LE.x;
                leftEyeP2.y=eyesInformation.LE.y;
                rightEyeP2.x=eyesInformation.RE.x;
                rightEyeP2.y=eyesInformation.RE.y;
                double yvalue=rightEyeP2.y-leftEyeP2.y;
                double xvalue=rightEyeP2.x-leftEyeP2.x;
                inAngle= atan(yvalue/xvalue);



                leftEyeP1.x=eyesInformation.LE.x-faceInformation.LT.x;
                leftEyeP1.y=eyesInformation.LE.y-faceInformation.LT.y-(clipFaceImage->height)/8;
                rightEyeP1.x=eyesInformation.RE.x-faceInformation.LT.x -((gray->width)/2);
                rightEyeP1.y=eyesInformation.RE.y-faceInformation.LT.y-(clipFaceImage->height)/8;



                lengthEye=eyesInformation.Length;
                prevlengthEye=eyesInformation.Length;
                widthEyeWindow=gray->width/2;
                heightEyeWindow=gray->height;

                IplImage* grayIm1 = cvCreateImage(cvSize(gray->width/2,gray->height),8,1);
                cvSetImageROI(gray,cvRect(0,0,(gray->width)/2,(gray->height)));
                cvResize(gray, grayIm1, CV_INTER_LINEAR ) ;
                cvResetImageROI(gray);
                leftEye.setModel(grayIm1);
                leftEye.anchorPoint=leftEyeP1;
                IplImage* grayIm2 = cvCreateImage(cvSize(gray->width/2,gray->height),8,1);
                cvSetImageROI(gray,cvRect(gray->width/2,0,gray->width/2,gray->height));
                cvResize(gray,grayIm2, CV_INTER_LINEAR ) ;
                cvResetImageROI(gray);
                rightEye.setModel(grayIm2);
                rightEye.anchorPoint=rightEyeP1;
                cvReleaseImage(&grayIm1);
                cvReleaseImage(&grayIm2);
                cvReleaseImage(&gray);

            }
            else
            {

                messageIndex=2;

            }
            cvReleaseImage(&clipFaceImage);

        }


    }

    else
    {
        if (flag!=1)
            messageIndex=2;

    }


    int newWidth,newHeight;
    if (flag==1)
    {
        newWidth=  floor((prevlengthEye*widthEyeWindow)/(lengthEye));
        newHeight=  floor((prevlengthEye*heightEyeWindow)/(lengthEye));
    }
//printf(" %d %d  JLK      DOSHDIUHSIUDSHIUDSSIUDHISDHSIUDH \n",newHeight,newWidth);

    if (flag==1 && newWidth>0 && newHeight>0 && prevlengthEye>0)
    {

        int lxdiff=0,rxdiff=0,lydiff=0,rydiff=0;
        CvMat *rotateMatrix = cvCreateMat(2, 3, CV_32FC1);
        double yvalue= rightEyeP.y- leftEyeP.y;
        double xvalue= rightEyeP.x-leftEyeP.x;
        double currentAngle = atan(yvalue/xvalue)*(180/CV_PI);

        currentAngle-=inAngle;
        if (isnan(currentAngle))
        {
            //         printf("%e  %e ANGLEEE\n",yvalue,xvalue);

        }
        // printf("%e ANGLEEE\n",currentAngle);
        CvPoint2D32f centre;
        centre.x = leftEyeP.x;
        centre.y = leftEyeP.y;
        cv2DRotationMatrix(centre, currentAngle, 1.0, rotateMatrix);
        IplImage *dstimg = cvCreateImage( cvSize(input->width,input->height), 8, input->nChannels );
        cvWarpAffine(input,dstimg,rotateMatrix,CV_WARP_FILL_OUTLIERS,cvScalarAll(0));
//cvNamedWindow("src",1);
        CvPoint rotatedRightP;
        rotatedRightP.x= floor( rightEyeP.x*CV_MAT_ELEM(*rotateMatrix, float, 0, 0) +  rightEyeP .y*CV_MAT_ELEM(*rotateMatrix, float, 0, 1) +CV_MAT_ELEM(*rotateMatrix, float, 0, 2));
        rotatedRightP.y= floor(rightEyeP.x*CV_MAT_ELEM(*rotateMatrix, float, 1, 0) +  rightEyeP.y*CV_MAT_ELEM(*rotateMatrix, float, 1, 1) +CV_MAT_ELEM(*rotateMatrix, float, 1, 2));
//printf("%d %d  VAAAAAAAAA\n",rotatedRightP.x,rotatedRightP.y);
        rightEyeP.x=rotatedRightP.x;
        rightEyeP.y=rotatedRightP.y;
        cvCircle(dstimg, rotatedRightP, 4, CV_RGB(128,128,128), 1, 8, 0 );

//cvShowImage("src",dstimg);

//cvWaitKey(1);

        int newWidthR,newHeightR,newWidthL,newHeightL;
        newWidthR=newWidth;

        newHeightR=newHeight;
        newWidthL=newWidth;
        newHeightL=newHeight;

        int ly=leftEyeP.y -int(floor(((leftEyeP1.y)*newWidth)/widthEyeWindow));
        int lx=leftEyeP.x -int(floor(((leftEyeP1.x)*newWidth)/widthEyeWindow));

        if (lx<0)
        {
            lxdiff=-lx;
            lx=0;

        }
        if (ly<0)
        {
            ly=0;
            lydiff=-ly;
        }
        if ((lx+newWidth)>320)
        {
            newWidthL=320-lx;
        }

        if ((ly+newHeight)>240)
        {

            newHeightL=240-ly;
        }

        IplImage *grayIm1 = cvCreateImage( cvSize(newWidthL,newHeightL), 8, 1 );
        cvSetImageROI(dstimg,cvRect(lx,ly,newWidthL,newHeightL));
        cvCvtColor( dstimg, grayIm1, CV_BGR2GRAY );
        cvResetImageROI(dstimg);
        int rx=rightEyeP.x -int(floor(((rightEyeP1.x)*newWidth)/widthEyeWindow));
        int ry=rightEyeP.y -int(floor(((rightEyeP1.y)*newWidth)/widthEyeWindow));
        //printf("%d %d %d %d   R    DOSHDIUHSIUDSHIUDSSIUDHISDHSIUDH \n",rx,ry,newHeight,newWidth);


        if (ry<0)
        {
            rydiff=-ry;
            ry=0;
        }
        if (rx<0)
        {
            rxdiff=-rx;
            rx=0;
        }
        if ((rx+newWidth)>320)
        {
            newWidthR=320-rx;
        }

        if ((ry+newHeight)>240)
        {

            newHeightR=240-ry;
        }

        IplImage *grayIm2 = cvCreateImage( cvSize(newWidthR,newHeightR), 8, 1 );
        cvSetImageROI(dstimg,cvRect(rx,ry,newWidthR,newHeightR));
        cvCvtColor( dstimg, grayIm2, CV_BGR2GRAY );
        cvResetImageROI(dstimg);

        leftEye.trackImage(grayIm1);
        rightEye.trackImage(grayIm2);
        CvPoint temp;
        leftEye.findPoint(leftEyeP1,&temp);
        CvPoint leftEyePTemp,leftEyeP1Temp,rightEyePTemp,rightEyeP1Temp;
        leftEyePTemp.y=leftEyeP.y -(((leftEyeP1.y)*newWidth)/widthEyeWindow)+ lydiff + temp.y;
        leftEyePTemp.x=leftEyeP.x -(((leftEyeP1.x)*newWidth)/widthEyeWindow)+ lxdiff +temp.x;
        leftEyeP1Temp.x=temp.x;
        leftEyeP1Temp.y=temp.y;
        rightEye.findPoint(rightEyeP1,&temp);
        rightEyePTemp.y=rightEyeP.y -(((rightEyeP1.y)*newWidth)/widthEyeWindow)+ rydiff+ temp.y;
        rightEyePTemp.x=rightEyeP.x -(((rightEyeP1.x)*newWidth)/widthEyeWindow)+ rxdiff+temp.x;
        rightEyeP1Temp.x=temp.x;
        rightEyeP1Temp.y=temp.y;
        double angle=atan(double(rightEyePTemp.y-leftEyePTemp.y)/double(rightEyePTemp.x-leftEyePTemp.x))*180/CV_PI;
        double angle2=atan(double(rightEyeP.y-leftEyeP.y)/double(rightEyeP.x-leftEyeP.x))*180/CV_PI;
        double v1,v2;
        v1=sqrt(pow(rightEyePTemp.y- rightEyeP.y,2)+ pow(rightEyePTemp.x-rightEyeP.x,2));
        v2=sqrt(pow(leftEyePTemp.y- leftEyeP.y,2)+ pow(leftEyePTemp.x-leftEyeP.x,2));
        double v3=(v1+v2);
        double lengthTemp=sqrt(pow(rightEyePTemp.y- leftEyePTemp.y,2)+ pow(rightEyePTemp.x-leftEyePTemp.x,2));

        if (pow((angle2-angle),2)<300 && v1<140 && v2 <144 && lengthTemp>1)
        {
            messageIndex=4;
            leftEyeP=leftEyePTemp;
            rightEyeP=rightEyePTemp;
            cv2DRotationMatrix(centre, -currentAngle, 1.0, rotateMatrix);
            CvPoint antiRotateR,antiRotateL;
            antiRotateR.x= floor( rightEyeP.x*CV_MAT_ELEM(*rotateMatrix, float, 0, 0) +  rightEyeP .y*CV_MAT_ELEM(*rotateMatrix, float, 0, 1) +CV_MAT_ELEM(*rotateMatrix, float, 0, 2));
            antiRotateR.y= floor(rightEyeP.x*CV_MAT_ELEM(*rotateMatrix, float, 1, 0) +  rightEyeP.y*CV_MAT_ELEM(*rotateMatrix, float, 1, 1) +CV_MAT_ELEM(*rotateMatrix, float, 1, 2));
            antiRotateL.x= floor( leftEyeP.x*CV_MAT_ELEM(*rotateMatrix, float, 0, 0) +  leftEyeP .y*CV_MAT_ELEM(*rotateMatrix, float, 0, 1) +CV_MAT_ELEM(*rotateMatrix, float, 0, 2));
            antiRotateL.y= floor(leftEyeP.x*CV_MAT_ELEM(*rotateMatrix, float, 1, 0) +  leftEyeP.y*CV_MAT_ELEM(*rotateMatrix, float, 1, 1) +CV_MAT_ELEM(*rotateMatrix, float, 1, 2));
            leftEyeP=antiRotateL;
            rightEyeP=antiRotateR;

            eyesInformation.LE.x=leftEyeP.x;
            eyesInformation.LE.y=leftEyeP.y;
            eyesInformation.RE.x=rightEyeP.x;
            eyesInformation.RE.y=rightEyeP.y;

            eyesInformation.Length=sqrt(pow(eyesInformation.RE.y-eyesInformation.LE.y,2)+ pow(eyesInformation.RE.x-eyesInformation.LE.x,2));

            prevlengthEye=eyesInformation.Length;
            //cvLine(input, eyesInformation.LE, eyesInformation.RE, cvScalar(0,255,0), 4);

        }
        cvReleaseImage(&dstimg);
        cvReleaseImage(&grayIm1);
        cvReleaseImage(&grayIm2);
        cvReleaseMat(&rotateMatrix);
    }

    if (checkEyeDetected()==1 && checkFaceDetected()==1 && boolClipFace==1 )
    {
        if (clipFaceCounter>0)
        {
            clippedFace[totalFaceClipNum-clipFaceCounter]=clipFace(input);
            clipFaceCounter--;
            messageIndex=5;
            sprintf(messageCaptureMessage,"Varied Expression. Better Recognition. Captured %d/%d faces.",totalFaceClipNum-clipFaceCounter+1,totalFaceClipNum);
            if(clipFaceCounter==0)
            {
                messageIndex=6;
                finishedClipFaceFlag=1;

            }
            //printf("aaaaaaaaaaaaa \n");
        }



    }

    return 0;
}
int detector::finishedClipFace()
{
  if(totalFaceClipNum>0 && finishedClipFaceFlag==1)
  {
    finishedClipFaceFlag=0;
       return 1;

  }
   else
   return 0;
}

IplImage ** detector::returnClipedFace()
{
    IplImage**temp =clippedFace;
    clippedFace=0;

    return temp;
}
void detector::startClipFace(int num)
{
    clippedFace =new IplImage * [num];
    totalFaceClipNum=num;
    clipFaceCounter=num;
    boolClipFace=1;
}

void detector::stopClipFace()
{
    totalFaceClipNum=0;
    clipFaceCounter=0;
 boolClipFace=0;
finishedClipFaceFlag=0;
    int i=0;
    for (i=0;i<totalFaceClipNum;i++)
    {
        if (clippedFace[i]!=0)
            cvReleaseImage(&clippedFace[i]);
    }
    if (clippedFace!=0)
        delete [] clippedFace;


}
int detector::detectorSuccessful()
{
    if (messageIndex==4)
        return 1;

    return 0;
}
char * detector::queryMessage()
{
    char *message0="Please come closer to the camera.";
    char *message1="Please go little far from the camera.";
    char *message2="Unable to Detect Your Face.";
    char *message3="Tracker lost, trying to reinitialize.";
    char *message4="Tracking in progress.";
    char *message6="TCapturing Image Finished.";

    if (messageIndex==-1)
        return 0;
    else if (messageIndex==0)
        return message0;
    else if (messageIndex==1)
        return message1;
    else if (messageIndex==2)
        return message2;
    else if (messageIndex==3)
        return message3;
    else if (messageIndex==4)
        return message4;
    else if (messageIndex==5)
    return messageCaptureMessage;
    else if (messageIndex==6)
    return message6;
}


