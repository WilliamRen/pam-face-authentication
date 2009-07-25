#include <verifier.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h> /* getpwdid */
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pam_face_defines.h"
#include "utils.h"

#include <iostream>
#include <list>
#include <string>
#include <cctype>


#define FACE_MACE_SIZE 64
#define EYE_MACE_SIZE 64
#define INSIDE_FACE_MACE_SIZE 64


using namespace std;

char * verifier::createSetDir()
{
    time_t ltime;
    struct tm *Tm;
    struct timeval detail_time;

    char* setDir  = new char[200];
    char* uniqueName  = new char[200];
    ltime=time(NULL);
    Tm=localtime(&ltime);
    gettimeofday(&detail_time,NULL);
    sprintf(uniqueName,"%d%d%d%d%d%d%d%d%d",Tm->tm_year,Tm->tm_mon,Tm->tm_mday,Tm->tm_hour,Tm->tm_min,Tm->tm_sec,(detail_time.tv_usec/1000),detail_time.tv_usec,Tm->tm_wday);
    sprintf(setDir,"%s/.pam-face-authentication/faces/%s", userStruct->pw_dir,uniqueName);
    mkdir(setDir, S_IRWXU );
    return uniqueName;
}
verifier::verifier()
{
    uid_t   userID=getuid();
    userStruct=getpwuid(getuid());
    char init[300];
    sprintf(init,"%s/.pam-face-authentication", userStruct->pw_dir);

    sprintf(facesDirectory,"%s/.pam-face-authentication/faces", userStruct->pw_dir);
    sprintf(modelDirectory,"%s/.pam-face-authentication/model", userStruct->pw_dir);
    sprintf(configDirectory,"%s/.pam-face-authentication/config", userStruct->pw_dir);
    char maceConfig[300];
    sprintf(maceConfig,"%s/mace.xml", configDirectory);
    mkdir(init, S_IRWXU );
    mkdir(facesDirectory, S_IRWXU );
    mkdir(modelDirectory, S_IRWXU );
    mkdir(configDirectory, S_IRWXU );
    FILE* fp = fopen(maceConfig, "r");
    if (fp)
    {
        fclose(fp);
    }
    else
    {
// Default Config

        config  newConfig;
        newConfig.filterMaceEyePCER=MACE_EYE_DEFAULT;
        newConfig.filterMaceFacePCER=MACE_FACE_DEFAULT;
        newConfig.filterMaceInsideFacePCER=MACE_INSIDE_FACE_DEFAULT;
        setConfig(&newConfig,configDirectory);
    }

}


verifier::verifier(uid_t   userID)
{
    userStruct=getpwuid(userID);
    sprintf(facesDirectory,"%s/.pam-face-authentication/faces", userStruct->pw_dir);
    sprintf(modelDirectory,"%s/.pam-face-authentication/model", userStruct->pw_dir);
    sprintf(configDirectory,"%s/.pam-face-authentication/config", userStruct->pw_dir);
    char maceConfig[300];
    sprintf(maceConfig,"%s/mace.xml", configDirectory);

    mkdir(facesDirectory, S_IRWXU );
    mkdir(modelDirectory, S_IRWXU );
    mkdir(configDirectory, S_IRWXU );
}

void verifier::createMaceFilter()
{
    //cvNamedWindow("eyeRight",1);

    setFace* temp=getFaceSet();
    int totalCount=0;
    for (int i=0;i<temp->count;i++)
    {
        totalCount+=temp->faceImages[i].count;
    }
    if (totalCount>0)
    {
        config newConfig;

        IplImage ** faceImage=new IplImage *[totalCount];
        int indexImage=0;
        for (int i=0;i<temp->count;i++)
        {
            for (int j=0;j<temp->faceImages[i].count;j++)
            {

                faceImage[indexImage]=temp->faceImages[i].faces[j];
                indexImage++;
            }
        }
        list<int> * maceValuesPSLR=new list<int>;
        list<double> * maceValues=new list<double>;
        list<double>::iterator it;
        IplImage ** eye=new IplImage *[totalCount];
        IplImage ** insideFace=new IplImage *[totalCount];
        CvMat *maceFilterVisualize;
        CvFileStorage* fs ;
        char modelname[300];
        sprintf(modelname,"%s/.pam-face-authentication/model/face_mace.xml", userStruct->pw_dir);
        int index=0;



        maceFilterVisualize=computeMace(faceImage,totalCount,FACE_MACE_SIZE);


        fs = cvOpenFileStorage( modelname, 0, CV_STORAGE_WRITE );
        cvWrite( fs, "maceFilter", maceFilterVisualize, cvAttrList(0,0) );
        cvReleaseFileStorage( &fs );
        indexImage=0;

        for (int i=0;i<temp->count;i++)
        {
            for (int j=0;j<(temp->faceImages[i].count);j++)
            {

                double macePCERValue=peakCorrPlaneEnergy(maceFilterVisualize,faceImage[indexImage],FACE_MACE_SIZE);
                int macePSLRValue=peakToSideLobeRatio(maceFilterVisualize,faceImage[indexImage],FACE_MACE_SIZE);
                maceValuesPSLR->push_back (macePSLRValue);
                maceValues->push_back (macePCERValue);

                //             printf("%d \n",macePSLRValue);

                indexImage++;
            }
        }
        maceValuesPSLR->sort();
        maceValues->sort();
        newConfig.filterMaceFacePCER=  maceValues->front();
        newConfig.filterMaceFacePSLR=  maceValuesPSLR->front();

        //   printf("%d %d \n",maceValues->front(), newConfig.filterMaceFacePCER);



        cvReleaseMat( &maceFilterVisualize );

        sprintf(modelname,"%s/.pam-face-authentication/model/eye_mace.xml", userStruct->pw_dir);
        for (index=0;index<totalCount;index++)
        {
            eye[index]=cvCreateImage(cvSize(EYE_MACE_SIZE,EYE_MACE_SIZE),8, faceImage[index]->nChannels);
            cvSetImageROI( faceImage[index],cvRect(0,0,140,60));
            cvResize(  faceImage[index], eye[index], CV_INTER_LINEAR );
            cvResetImageROI( faceImage[index]);

        }
        maceFilterVisualize=computeMace(eye,totalCount,EYE_MACE_SIZE);
        maceValues->clear();
        maceValuesPSLR->clear();

        indexImage=0;

        for (int i=0;i<temp->count;i++)
        {
            for (int j=0;j<(temp->faceImages[i].count);j++)
            {
                double macePCERValue=peakCorrPlaneEnergy(maceFilterVisualize,eye[indexImage],EYE_MACE_SIZE);
                int macePSLRValue=peakToSideLobeRatio(maceFilterVisualize,eye[indexImage],EYE_MACE_SIZE);
                maceValuesPSLR->push_back (macePSLRValue);
                maceValues->push_back (macePCERValue);
                //                 printf("%d \n",macePSLRValue);

                indexImage++;
            }
        }
        maceValuesPSLR->sort();
        maceValues->sort();
        newConfig.filterMaceEyePCER= maceValues->front();
        newConfig.filterMaceEyePSLR= maceValuesPSLR->front();

        // printf("%d %d \n",maceValues->front(), newConfig.filterMaceEyePCER);

        fs = cvOpenFileStorage( modelname, 0, CV_STORAGE_WRITE );
        cvWrite( fs, "maceFilter", maceFilterVisualize, cvAttrList(0,0) );
        cvReleaseFileStorage( &fs );
        cvReleaseMat( &maceFilterVisualize );


        sprintf(modelname,"%s/.pam-face-authentication/model/inside_face_mace.xml", userStruct->pw_dir);

        for (index=0;index<totalCount;index++)
        {

            insideFace[index]=cvCreateImage(cvSize(INSIDE_FACE_MACE_SIZE,INSIDE_FACE_MACE_SIZE),8, faceImage[index]->nChannels);

            cvSetImageROI( faceImage[index],cvRect(30,45,80,105));
            cvResize( faceImage[index], insideFace[index], CV_INTER_LINEAR );
            cvResetImageROI( faceImage[index]);



            cvResetImageROI(faceImage[index]);
        }
        maceFilterVisualize=computeMace(insideFace,totalCount,INSIDE_FACE_MACE_SIZE);

        indexImage=0;
        maceValues->clear();
        maceValuesPSLR->clear();
        for (int i=0;i<temp->count;i++)
        {
            for (int j=0;j<(temp->faceImages[i].count);j++)
            {

                double macePCERValue=peakCorrPlaneEnergy(maceFilterVisualize,insideFace[indexImage],INSIDE_FACE_MACE_SIZE);
                int macePSLRValue=peakToSideLobeRatio(maceFilterVisualize,insideFace[indexImage],INSIDE_FACE_MACE_SIZE);
                maceValuesPSLR->push_back (macePSLRValue);
                //  printf("%d \n",macePSLRValue);
                maceValues->push_back (macePCERValue);
                indexImage++;
            }
        }
        maceValues->sort();
        maceValuesPSLR->sort();

        newConfig.filterMaceInsideFacePSLR=maceValuesPSLR->front();
        newConfig.filterMaceInsideFacePCER=maceValues->front();

        // printf("%d %d \n",maceValues->front(), newConfig.filterMaceInsideFacePCER);
        setConfig(&newConfig,configDirectory);


        fs = cvOpenFileStorage( modelname, 0, CV_STORAGE_WRITE );
        cvWrite( fs, "maceFilter", maceFilterVisualize, cvAttrList(0,0) );
        cvReleaseFileStorage( &fs );
        cvReleaseMat( &maceFilterVisualize );



        for (index=0;index<totalCount;index++)
        {
            cvReleaseImage(&faceImage[index]);
            cvReleaseImage(&eye[index]);
            cvReleaseImage(&insideFace[index]);


        }
        delete [] faceImage;
        delete [] eye;
        delete [] insideFace;

    }

}
void verifier::addFaceSet(IplImage **set,int size)
{
    char* dirNameUnique=createSetDir();
    char  dirName[300];
    sprintf(dirName,"%s/.pam-face-authentication/faces/%s", userStruct->pw_dir,dirNameUnique);


    int i=0;
    for (i=0;i<size;i++)
    {
        char filename[300];
        sprintf(filename,"%s/%d.jpg",dirName,i);
        cvSaveImage(filename,set[i]);
        cvReleaseImage(&set[i]);
    }

    createMaceFilter();
    delete [] set;
}

void verifier::removeFaceSet(char* setName)
{
    char dirname[300],filename[300];
    struct dirent *de=NULL;
    DIR *d=NULL;
    sprintf(dirname,"%s/%s",facesDirectory,setName);

    d=opendir(dirname);
    while (de= readdir(d))
    {

        if (strcmp(de->d_name+strlen(de->d_name)-3, "jpg")==0)
        {

            sprintf(filename,"%s/%s",dirname,de->d_name);
            // printf("%s \n",filename);
            remove(filename);
        }
    }
    //  printf("No seggy \n");
    //  char maceFilters[300];
    //  sprintf(maceFilters,"%s/%s_face_mace.xml",modelDirectory,setName);
//   remove(maceFilters);
    //  sprintf(maceFilters,"%s/%s_eye_mace.xml",modelDirectory,setName);
//   remove(maceFilters);
    //  sprintf(maceFilters,"%s/%s_inside_face_mace.xml",modelDirectory,setName);
    //  remove(maceFilters);


    remove(dirname);
    createMaceFilter();
    //sprintf(dirname,"%s/%s_mace.xml",modelDirectory,setName);
    //remove(dirname);

}




int verifier::verifyFace(IplImage *faceMain)
{
    if (faceMain==0)
        return 0;
    char temp[300];





    sprintf(temp,"%s/face_mace.xml",modelDirectory);
    if (file_exists(temp)==0)
        return 0;
    IplImage * face= cvCreateImage( cvSize(140,150),8,faceMain->nChannels);
    cvResize( faceMain,face, CV_INTER_LINEAR ) ;
    struct dirent *de=NULL;
    DIR *d=NULL;
    d=opendir(modelDirectory);
    int k=0;
    while (de = readdir(d))
    {
        if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
        {
            if ((strcmp(de->d_name +strlen(de->d_name)-8, "mace.xml")==0))
            {
                k++;
                //  mylist->push_back (de->d_name);
            }
        }
    }

    if (k<3)
        return 0;


    IplImage * eye=cvCreateImage(cvSize(EYE_MACE_SIZE,EYE_MACE_SIZE),8,face->nChannels);
    cvSetImageROI(face,cvRect(0,0,140,60));
    cvResize(face, eye, CV_INTER_LINEAR );
    cvResetImageROI(face);


    IplImage * insideFace=cvCreateImage(cvSize(INSIDE_FACE_MACE_SIZE,INSIDE_FACE_MACE_SIZE),8,face->nChannels);
    cvSetImageROI(face,cvRect(30,45,80,105));
    cvResize(face, insideFace, CV_INTER_LINEAR );
    cvResetImageROI(face);

    config * newConfig=getConfig(configDirectory);
    char * macefilternamepath=new char[300];
    CvFileStorage * fileStorage;
    CvMat *maceFilterUser;


    sprintf(macefilternamepath,"%s/%s",modelDirectory,"face_mace.xml");
    fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
    maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);
    double faceValuePCER =peakCorrPlaneEnergy(maceFilterUser,face,FACE_MACE_SIZE);
   int faceValuePSLR =peakToSideLobeRatio(maceFilterUser,face,FACE_MACE_SIZE);

    cvReleaseFileStorage( &fileStorage );

    cvReleaseMat( &maceFilterUser );




    sprintf(macefilternamepath,"%s/%s",modelDirectory,"eye_mace.xml");
    fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
    maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);

    double eyeValuePCER =peakCorrPlaneEnergy(maceFilterUser,eye,EYE_MACE_SIZE);
   int eyeValuePSLR =peakToSideLobeRatio(maceFilterUser,eye,EYE_MACE_SIZE);



    cvReleaseFileStorage( &fileStorage );
    cvReleaseMat( &maceFilterUser );

    sprintf(macefilternamepath,"%s/%s",modelDirectory,"inside_face_mace.xml");
    fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
    maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);

    double insideFaceValuePCER=peakCorrPlaneEnergy(maceFilterUser,insideFace,INSIDE_FACE_MACE_SIZE);
   int  insideFaceValuePSLR =peakToSideLobeRatio(maceFilterUser,insideFace,INSIDE_FACE_MACE_SIZE);


    //printf("%d PTSR of INSIDE FACE \n",v);
    cvReleaseFileStorage( &fileStorage );
    cvReleaseMat( &maceFilterUser );
    /*
      FILE* fp = fopen("/home/darksid3hack0r/value.txt", "a");

      if (fp)
      {
          fprintf(fp,"%d %d %d \n",faceValue,eyeValue,insideFaceValue);
      }
      fclose(fp);
      */

  //  printf("PCER The Values are  Face %e  Eye %e Nose+mouth  %e \n PSLR The Values are  Face %d  Eye %d Nose+mouth  %d \n",faceValuePCER,eyeValuePCER,insideFaceValuePCER,faceValuePSLR,eyeValuePSLR,insideFaceValuePSLR);

    int count=0;

   /* if (newConfig->filterMaceInsideFacePCER<=insideFaceValuePCER)
        count++;
    if (newConfig->filterMaceFacePCER<=faceValuePCER)
        count++;
    if (newConfig->filterMaceEyePCER<=eyeValuePCER)
        count++;
    */
     if(newConfig->filterMaceInsideFacePSLR<=insideFaceValuePSLR)
        count++;
    if (newConfig->filterMaceFacePSLR<=faceValuePSLR)
        count++;
    if (newConfig->filterMaceEyePSLR<=eyeValuePSLR)
        count++;



    if (count>1)
    {
        //printf("\n YES \n");
        return 1;
    }
    else
        return 0;



    /* list<string> * mylistFace=new list<string>;
     list<string> * mylistEye=new list<string>;
     list<string> * mylistInsideFace=new list<string>;

     list<string>::iterator it;

     IplImage * face= cvCreateImage( cvSize(140,150),8,faceMain->nChannels);
     cvResize( faceMain,face, CV_INTER_LINEAR ) ;
     struct dirent *de=NULL;
     DIR *d=NULL;
     d=opendir(modelDirectory);
     int k=0;
     while (de = readdir(d))
     {
         if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
         {
             // printf("%s \n",de->d_name);
             if (((strcmp(de->d_name +strlen(de->d_name)-14, "_face_mace.xml")==0) && !(strcmp(de->d_name +strlen(de->d_name)-20, "inside_face_mace.xml")==0)))
             {
                 k++;
                 mylistFace->push_back (de->d_name);
             }
             if ((strcmp(de->d_name +strlen(de->d_name)-20, "inside_face_mace.xml")==0))
             {
                 mylistInsideFace->push_back (de->d_name);
             }
             if ((strcmp(de->d_name +strlen(de->d_name)-12, "eye_mace.xml")==0))
             {
                 mylistEye->push_back (de->d_name);
             }
         }
     }

     if (k==0)
         return 0;



     IplImage * eye=cvCreateImage(cvSize(140,60),8,face->nChannels);
     cvSetImageROI(face,cvRect(0,0,140,60));
     cvResize(face, eye, CV_INTER_LINEAR );
     cvResetImageROI(face);

     IplImage * insideFace=cvCreateImage(cvSize(80,105),8,face->nChannels);
     cvSetImageROI(face,cvRect(30,45,80,105));
     cvResize(face, insideFace, CV_INTER_LINEAR );
     cvResetImageROI(face);

     config * newConfig=getConfig(configDirectory);
     char * macefilternamepath=new char[300];
     CvFileStorage * fileStorage;
     CvMat *maceFilterUser;

     int maxFaceValue=0,maxEyeValue=0,maxInsideFaceValue=0;

     for (it=mylistFace->begin(); it!=mylistFace->end(); ++it)
     {
         string l=*it;
         char *p;
         char * fileName=new char[300];
         p=&l[0];
         sprintf(fileName,"%s",p);
         sprintf(macefilternamepath,"%s/%s",modelDirectory,fileName);
         fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
         maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);
         int faceValue=peakToSideLobeRatio(maceFilterUser,face,FACE_MACE_SIZE);
         cvReleaseFileStorage( &fileStorage );
         cvReleaseMat( &maceFilterUser );
         maxFaceValue+=faceValue;

     }


     for (it=mylistEye->begin(); it!=mylistEye->end(); ++it)
     {
         string l=*it;
         char *p;
         char * fileName=new char[300];
         p=&l[0];
         sprintf(fileName,"%s",p);
         sprintf(macefilternamepath,"%s/%s",modelDirectory,fileName);
         fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
         maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);
         int eyeValue=peakToSideLobeRatio(maceFilterUser,eye,EYE_MACE_SIZE);
         cvReleaseFileStorage( &fileStorage );
         cvReleaseMat( &maceFilterUser );
         maxEyeValue+=eyeValue;
     }


     for (it=mylistInsideFace->begin(); it!=mylistInsideFace->end(); ++it)
     {
         string l=*it;
         char *p;
         char * fileName=new char[300];
         p=&l[0];
         sprintf(fileName,"%s",p);
         sprintf(macefilternamepath,"%s/%s",modelDirectory,fileName);
         fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
         maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);
         int insideFaceValue=peakToSideLobeRatio(maceFilterUser,insideFace,INSIDE_FACE_MACE_SIZE);
         //printf("%d PTSR of INSIDE FACE \n",v);
         cvReleaseFileStorage( &fileStorage );
         cvReleaseMat( &maceFilterUser );
         maxInsideFaceValue+=insideFaceValue;
     }

     maxFaceValue/=k;
     maxEyeValue/=k;
     maxInsideFaceValue/=k;
    */

    /*
      FILE* fp = fopen("/home/darksid3hack0r/value.txt", "a");

      if (fp)
      {
          fprintf(fp,"%d %d %d \n",faceValue,eyeValue,insideFaceValue);
      }
      fclose(fp);


    printf("The Values are  Face %d  Eye %d Nose+mouth  %d \n",maxFaceValue,maxEyeValue,maxInsideFaceValue);

    int count=0;
    if (newConfig->filterMaceInsideFacePCER<=maxInsideFaceValue)
        count++;
    if (newConfig->filterMaceFacePCER<=maxFaceValue)
        count++;
    if (newConfig->filterMaceEyePCER<=maxEyeValue)
        count++;


    if (count>1)
    {
        //printf("\n YES \n");
        return 1;
    }
    else
        return 0;

    */
}
/*
allFaces* verifier::getFaceImagesFromAllSet()
{
    setFace* faceSetStruct=getFaceSet();
    int i=0;
    char modelDir[300];
    struct dirent *de=NULL;
    DIR *d=NULL;
    list<string> * mylist=new list<string>;
    list<string>::iterator it;
    int k=0;
    for (i=0;i<faceSetStruct->count;i++)
    {
        sprintf(modelDir,"%s/%s",facesDirectory,faceSetStruct->setName[i]);
        // printf("%s \n",modelDir);
        d=opendir(modelDir);
        while (de = readdir(d))
        {
            if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
            {
                k++;
                char fullPath[300];
                sprintf(fullPath,"%s/%s",modelDir,de->d_name);
                mylist->push_back (fullPath);
            }
        }
        mylist->sort();
    }


    allFaces* newAllFaces=new allFaces;
    newAllFaces->count=0;
    if (k==0)
        return newAllFaces;
    newAllFaces->faceImages =new IplImage *[(int) mylist->size()];
    newAllFaces->count =(int) mylist->size();
    int index=0;

    for (it=mylist->begin(); it!=mylist->end(); ++it)
    {

        string l=*it;
        char *p;
        char * path=new char[300];
        p=&l[0];
        sprintf(path,"%s",p);
        newAllFaces->faceImages[index]=cvLoadImage(path,1);
        index++;

    }

    return newAllFaces;
}

*/


setFace* verifier::getFaceSet()
{

    struct dirent *de=NULL;
    DIR *d=NULL;
    list<string> * mylist=new list<string>;
    list<string>::iterator it;
    d=opendir(facesDirectory);

    int count=0;
    while (de = readdir(d))
    {
        if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
        {
            mylist->push_back (de->d_name);
            count++;

        }
    }

    mylist->sort();

    setFace* setFaceStruct= new setFace;
    setFaceStruct->setName =new char *[(int) mylist->size()];
    setFaceStruct->setFilePathThumbnails =new char *[(int) mylist->size()];
    setFaceStruct->faceImages = new structFaceImages[(int) mylist->size()];
    setFaceStruct->count=count;


///./setFaceStruct->faceImages->count=imageK;




    int k=0;
    for (it=mylist->begin(); it!=mylist->end(); ++it)
    {
        string l=*it;
        char *p;
        char * setName=new char[300];
        p=&l[0];
        sprintf(setName,"%s",p);
        setFaceStruct->setName[k]=setName;
        char * fileThumb=new char[300];
        sprintf(fileThumb,"%s/%s/1.jpg",facesDirectory,p);
        setFaceStruct->setFilePathThumbnails[k]=fileThumb;
        char imagesDir[300];
        sprintf(imagesDir,"%s/%s",facesDirectory,p);
        int imageK=0;

        struct dirent *de=NULL;
        DIR *d=NULL;
        list<string> * mylistImages=new list<string>;
        list<string>::iterator itImages;
        d=opendir(imagesDir);


        while (de = readdir(d))
        {
            if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
            {
                imageK++;
                char fullPath[300];
                sprintf(fullPath,"%s/%s",imagesDir,de->d_name);
                mylistImages->push_back (fullPath);
            }

        }

        mylistImages->sort();
        setFaceStruct->faceImages[k].faces= new IplImage* [imageK];
        setFaceStruct->faceImages[k].count=imageK;
        int imageIndex=0;
        for (itImages=mylistImages->begin(); itImages!=mylistImages->end(); ++itImages)
        {
            string l=*itImages;
            char *p;
            char * fileName=new char[300];
            p=&l[0];
            sprintf(fileName,"%s",p);
            setFaceStruct->faceImages[k].faces[imageIndex]=cvLoadImage(fileName,1);
            imageIndex++;
        }

        k++;
    }

    return setFaceStruct;
}
